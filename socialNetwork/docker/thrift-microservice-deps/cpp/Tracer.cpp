/*
 * Copyright (c) 2017 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jaegertracing/Tracer.h"
#include "jaegertracing/Reference.h"
#include "jaegertracing/TraceID.h"
#include "jaegertracing/samplers/SamplingStatus.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <iterator>
#include <opentracing/util.h>
#include <tuple>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <map>
#include <ctime>
#include <cstdlib>
#include <curl/curl.h>

namespace jaegertracing {
namespace {

using SystemClock = Tracer::SystemClock;
using SteadyClock = Tracer::SteadyClock;
using TimePoints = std::tuple<SystemClock::time_point, SteadyClock::time_point>;


TimePoints determineStartTimes(const opentracing::StartSpanOptions& options)
{
    if (options.start_system_timestamp == SystemClock::time_point() &&
        options.start_steady_timestamp == SteadyClock::time_point()) {
        return std::make_tuple(SystemClock::now(), SteadyClock::now());
    }
    if (options.start_system_timestamp == SystemClock::time_point()) {
        return std::make_tuple(opentracing::convert_time_point<SystemClock>(
                                   options.start_steady_timestamp),
                               options.start_steady_timestamp);
    }
    if (options.start_steady_timestamp == SteadyClock::time_point()) {
        return std::make_tuple(options.start_system_timestamp,
                               opentracing::convert_time_point<SteadyClock>(
                                   options.start_system_timestamp));
    }
    return std::make_tuple(options.start_system_timestamp,
                           options.start_steady_timestamp);
}

}  // anonymous namespace

std::map<std::string, double> span_states;
int first_time_initialize = 0;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, FILE* file) {
    return fwrite(contents, size, nmemb, file);
}

void fetch_span_states() {

        // std::ifstream file_in("astraea-spans");
	//
///////////
    std::cout <<"fetching s3 file";
    CURL* curl;
    CURLcode res;
    std::string url = "https://astrea-syed.s3.amazonaws.com/scratch_14.json"; // Replace with the URL of the file you want to download.
    std::string local_file_path = "/tmp/astrea-spans"; // Local file path where you want to save the downloaded file.

    FILE* file = fopen(local_file_path.c_str(), "wb");
    if (!file) {
        std::cerr << "Failed to open the local file for writing." << std::endl;
    }

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to write data to the local file.
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "cURL failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        fclose(file);

        if (res == CURLE_OK) {
            std::cout << "File downloaded successfully to: " << local_file_path << std::endl;
        } else {
            std::cout << "File download failed." << std::endl;
        }
    }
//////////
        std::cout <<"fetched s3 file";
        const char *fileName="/astraea-spans/spans";
        std::ifstream paramFile;
        paramFile.open(fileName);

        std::string line;
        std::string key;
        double value;

        while ( paramFile >> key >> value ) {
            span_states[key] = value; // input them into the map
        }
        paramFile.close();
}

void launch_astraea_daemon()
{

                for(unsigned j = 0; j < INT_MAX; ++j)
                {
                        fetch_span_states();
                        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

                }

}


using StrMap = SpanContext::StrMap;

constexpr int Tracer::kGen128BitOption;

std::unique_ptr<opentracing::Span>
Tracer::StartSpanWithOptions(string_view operationName,
                             const opentracing::StartSpanOptions& options) const
    noexcept
{
    try {

        // Tsl: Astraea logic embedded here
        bool isDisabled = false;


//         _logger->info("-----Mertiko info checking file");
//         _logger->info(operationName);

	
        if (first_time_initialize == 0){
                _logger->info("Launching a thread");
                std::thread t(launch_astraea_daemon);

                _logger->info("detaching the thread\n");
                t.detach();

                first_time_initialize = 1;
        }

        if (span_states.count(operationName) > 0){
// 		_logger->info("Yes operation name is given");
//                 std::cout << "mymap is " << span_states[operationName] << '\n';
                double span_sampling_probability = span_states[operationName];
                double dice = (double) rand()/RAND_MAX * 100;
//                 _logger->info(std::to_string(dice));
// 		_logger->info(std::to_string(span_sampling_probability));
                if ( dice > span_sampling_probability){
                        isDisabled = true;
                }
        }
	

        //  _logger->info("-----Mertiko info checking file");
        //  _logger->info(operationName);

        // std::ifstream fin("/astraea-spans/spans");
        // std::string s;

        // while (getline(fin,s)) {
        //     if (s.find(operationName) != std::string::npos) {
                
        //         _logger->info("+++Mertiko disabled");
        //         _logger->info(operationName);
        //         isDisabled = true;

        //     }
        // }
        



        const auto result = analyzeReferences(options.references);
        const auto* parent = result._parent;
        const auto& references = result._references;

        std::vector<Tag> samplerTags;
        auto newTrace = false;
        SpanContext ctx;
        // tsl: root span that we do not touch
        if (!parent || !parent->isValid()) {
            newTrace = true;
            auto highID = static_cast<uint64_t>(0);
            if (_options & kGen128BitOption) {
                highID = randomID();
            }
            const TraceID traceID(highID, randomID());
            const auto spanID = traceID.low();
            const auto parentID = 0;
            auto flags = static_cast<unsigned char>(0);
            if (parent && parent->isDebugIDContainerOnly()) {
                flags |=
                    (static_cast<unsigned char>(SpanContext::Flag::kSampled) |
                     static_cast<unsigned char>(SpanContext::Flag::kDebug));
            }
            else {
                const auto samplingStatus =
                    _sampler->isSampled(traceID, operationName);
                if (samplingStatus.isSampled()) {
                    flags |=
                        static_cast<unsigned char>(SpanContext::Flag::kSampled);
                    samplerTags = samplingStatus.tags();
                }
            }
            ctx = SpanContext(traceID, spanID, parentID, flags, StrMap());
        }
        // children sppan where we intercept
        else {
            
            const auto traceID = parent->traceID();
            const auto spanID = randomID();
            const auto parentID = parent->spanID();
	    //const auto flags = parent->flags();
            const auto parentparentID = parent->parentID();

            // std::cout << "*-*INFO-mert children: " << operationName <<'\n';

            //utils::ErrorUtil::logError(*_logger, "*-*INFO-mert2:");
            // _logger->info("Mertiko info2; spanId: " +  spanID + ", parentId: " + parentID + ", parentparent: "+ parentparentID);
//             _logger->info("---Mertiko info");
            if (isDisabled){
//                  _logger->info("+++Mertiko disabled");
                ctx = SpanContext(traceID, parentID, parentparentID, 0, StrMap());
            }
            else{
                ctx = SpanContext(traceID, spanID, parentID, 1, StrMap());
            }

            
        }

        if (parent && !parent->baggage().empty()) {
            ctx = ctx.withBaggage(parent->baggage());
//              _logger->info("+++Mertiko baggage");
            // std::cout << "*-*INFO-mert children with baggage: " << operationName <<'\n';
        }

        SystemClock::time_point startTimeSystem;
        SteadyClock::time_point startTimeSteady;
        std::tie(startTimeSystem, startTimeSteady) =
            determineStartTimes(options);



        // tsl: sleep
//         _logger->info("----Mertiko fixed sleep checking file");
        std::ifstream fin2("/astraea-spans/sleeps");
        std::string sleep;

        while (getline(fin2,sleep)) {
            if (sleep.find(operationName) != std::string::npos) {
                
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::default_random_engine generator(seed);
                
                std::normal_distribution<> d{5000,1000};
                int x = std::round(d(generator));
                if ( x < 0 )
                {
                    x = 0;
                }
		 _logger->info(operationName);
                 _logger->info("+++Mertiko sleep microseconds");
                 _logger->info(std::to_string(x));
		  
                std::this_thread::sleep_for(std::chrono::microseconds(x));
            }
        }

        return startSpanInternal(ctx,
                                 operationName,
                                 startTimeSystem,
                                 startTimeSteady,
                                 samplerTags,
                                 options.tags,
                                 newTrace,
                                 references);
    } catch (...) {
        utils::ErrorUtil::logError(
            *_logger, "Error occurred in Tracer::StartSpanWithOptions");
        return nullptr;
    }
}

std::unique_ptr<Span>
Tracer::startSpanInternal(const SpanContext& context,
                          const std::string& operationName,
                          const SystemClock::time_point& startTimeSystem,
                          const SteadyClock::time_point& startTimeSteady,
                          const std::vector<Tag>& internalTags,
                          const std::vector<OpenTracingTag>& tags,
                          bool newTrace,
                          const std::vector<Reference>& references) const
{
    std::vector<Tag> spanTags;
    spanTags.reserve(tags.size() + internalTags.size());
    std::transform(
        std::begin(tags),
        std::end(tags),
        std::back_inserter(spanTags),
        [](const OpenTracingTag& tag) { return Tag(tag.first, tag.second); });
    spanTags.insert(
        std::end(spanTags), std::begin(internalTags), std::end(internalTags));

    std::unique_ptr<Span> span(new Span(shared_from_this(),
                                        context,
                                        operationName,
                                        startTimeSystem,
                                        startTimeSteady,
                                        spanTags,
                                        references));

    _metrics->spansStarted().inc(1);
    if (span->context().isSampled()) {
        _metrics->spansSampled().inc(1);
        if (newTrace) {
            _metrics->tracesStartedSampled().inc(1);
        }
    }
    else {
        _metrics->spansNotSampled().inc(1);
        if (newTrace) {
            _metrics->tracesStartedNotSampled().inc(1);
        }
    }

    return span;
}

Tracer::AnalyzedReferences
Tracer::analyzeReferences(const std::vector<OpenTracingRef>& references) const
{
    AnalyzedReferences result;
    auto hasParent = false;
    const auto* parent = result._parent;
    for (auto&& ref : references) {
        const auto* ctx = dynamic_cast<const SpanContext*>(ref.second);

        if (!ctx) {
            _logger->error("Reference contains invalid type of SpanReference");
            continue;
        }

        if (!ctx->isValid() && !ctx->isDebugIDContainerOnly() &&
            ctx->baggage().empty()) {
            continue;
        }

        result._references.emplace_back(Reference(*ctx, ref.first));

        if (!hasParent) {
            parent = ctx;
            hasParent =
                (ref.first == opentracing::SpanReferenceType::ChildOfRef);
        }
    }

    if (!hasParent && parent && parent->isValid()) {
        // Use `FollowsFromRef` in place of `ChildOfRef`.
        hasParent = true;
    }

    if (hasParent) {
        assert(parent);
        result._parent = parent;
    }

    return result;
}

}  // namespace jaegertracing
