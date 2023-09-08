/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <alibabacloud/oss/http/HttpRequest.h>
#include <alibabacloud/oss/http/HttpResponse.h>
#include <async_simple/coro/Lazy.h>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <ylt/coro_http/coro_http_client.hpp>

namespace AlibabaCloud {
namespace OSS {

class ALIBABACLOUD_OSS_EXPORT HttpClient {
public:
  HttpClient();
  virtual ~HttpClient();

  virtual std::shared_ptr<HttpResponse>
  makeRequest(const std::shared_ptr<HttpRequest> &request) = 0;

  virtual async_simple::coro::Lazy<std::shared_ptr<HttpResponse>>
  makeRequestAsync(const std::shared_ptr<HttpRequest> &) {
    co_return nullptr;
  }

  bool isEnable();
  void disable();
  void enable();
  void waitForRetry(long milliseconds);

protected:
  std::atomic<bool> disable_;
  std::mutex requestLock_;
  std::condition_variable requestSignal_;
};
} // namespace OSS
} // namespace AlibabaCloud
