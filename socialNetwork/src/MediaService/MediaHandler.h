#ifndef SOCIAL_NETWORK_MICROSERVICES_SRC_MEDIASERVICE_MEDIAHANDLER_H_
#define SOCIAL_NETWORK_MICROSERVICES_SRC_MEDIASERVICE_MEDIAHANDLER_H_

#include <chrono>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <fstream>

#include "../../gen-cpp/MediaService.h"
#include "../logger.h"
#include "../tracing.h"

// 2018-01-01 00:00:00 UTC
#define CUSTOM_EPOCH 1514764800000

namespace social_network {

class MediaHandler : public MediaServiceIf {
 public:
  MediaHandler() = default;
  ~MediaHandler() override = default;

  void ComposeMedia(std::vector<Media> &_return, int64_t,
                    const std::vector<std::string> &,
                    const std::vector<int64_t> &,
                    const std::map<std::string, std::string> &) override;

 private:
};

void MediaHandler::ComposeMedia(
    std::vector<Media> &_return, int64_t req_id,
    const std::vector<std::string> &media_types,
    const std::vector<int64_t> &media_ids,
    const std::map<std::string, std::string> &carrier) {
  // Initialize a span
  TextMapReader reader(carrier);
  std::map<std::string, std::string> writer_text_map;
  TextMapWriter writer(writer_text_map);
  auto parent_span = opentracing::Tracer::Global()->Extract(reader);
  auto span = opentracing::Tracer::Global()->StartSpan(
      "compose_media_server", {opentracing::ChildOf(parent_span->get())});
  opentracing::Tracer::Global()->Inject(span->context(), writer);
    // std::ifstream fin("/astraea-spans/statesds");
    // std::string s;

    // while (getline(fin,s)) {
    //     if (s.find("compose_media_server") != std::string::npos) {
            
    //         // sleep now
    //             unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    //           std::default_random_engine generator(seed);
              
    //           std::normal_distribution<> d{100,30};
    //           int x = std::round(d(generator));
    //           // cout<<x;
    //           LOG(info) << "*Mert compose_media_server sleep*";
    //           LOG(info) << x;
    //           std::this_thread::sleep_for(std::chrono::microseconds(x));
    //     }
    // }
    // introduce random sleep
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    std::default_random_engine generator(seed);
//    std::normal_distribution<> d{10000,3000};
//    int x = std::round(d(generator));
//    LOG(info) << "*Mert compose_media_server sleep*";
//    LOG(info) << x;
//    std::this_thread::sleep_for(std::chrono::microseconds(x));

  if (media_types.size() != media_ids.size()) {
    ServiceException se;
    se.errorCode = ErrorCode::SE_THRIFT_HANDLER_ERROR;
    se.message =
        "The lengths of media_id list and media_type list are not equal";
    throw se;
  }

  for (int i = 0; i < media_ids.size(); ++i) {
    Media new_media;
    new_media.media_id = media_ids[i];
    new_media.media_type = media_types[i];
    _return.emplace_back(new_media);
  }

  span->Finish();
}

}  // namespace social_network

#endif  // SOCIAL_NETWORK_MICROSERVICES_SRC_MEDIASERVICE_MEDIAHANDLER_H_
