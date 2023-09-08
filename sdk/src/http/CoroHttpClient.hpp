#pragma once

#include <alibabacloud/oss/client/ClientConfiguration.h>
#include <alibabacloud/oss/http/HttpClient.h>
#include <async_simple/coro/SyncAwait.h>
#include <cstddef>
#include <fstream>
#include <ios>

namespace AlibabaCloud::OSS {
class CoroHttpClient : public HttpClient {
public:
  CoroHttpClient(const ClientConfiguration &) : HttpClient() {}
  ~CoroHttpClient() {}

  virtual std::shared_ptr<HttpResponse>
  makeRequest(const std::shared_ptr<HttpRequest> &request) override {
    std::cout << request->method() << "\n";
    // size_t content_len = 0;
    auto &headers = request->Headers();
    for (auto &[k, v] : headers) {
      if (k == "Content-Length") {
        // content_len = atoi(v.data());
      }
      client_.add_header(k, v);
    }

    // auto body = request->Body();
    // std::string str;
    // str.resize(content_len);
    // body->read(str.data(), content_len);
    // std::ofstream file("temp.xml", std::ios::binary);
    // file.write(str.data(), str.size());
    // file.close();

    return async_simple::coro::syncAwait(makeRequestAsync(request));
  }

  virtual async_simple::coro::Lazy<std::shared_ptr<HttpResponse>>
  makeRequestAsync(const std::shared_ptr<HttpRequest> &request) override {
    auto url = request->url();
    std::string str_url;
    str_url.append(url.scheme())
        .append("://")
        .append(url.host())
        .append(url.path());

    // auto result = co_await client_.async_upload_chunked(
    //     str_url, coro_http::http_method::PUT, "temp.xml",
    //     coro_http::req_content_type::xml);
    // std::cout << result.status << "\n";
    // std::cout << result.resp_body << "\n";

    auto result = co_await client_.async_upload_chunked(
        str_url, coro_http::http_method::PUT, request->Body(),
        coro_http::req_content_type::xml);
    std::cout << result.status << "\n";
    std::cout << result.resp_body << "\n";

    auto response = std::make_shared<HttpResponse>(request);
    response->setStatusCode(result.status);

    for (auto [k, v] : result.resp_headers) {
      std::cout << k << ": " << v << "\n";
      response->setHeader(std::string(k), std::string(v));
    }

    co_return response;
  }

private:
  coro_http::coro_http_client client_;
};
} // namespace AlibabaCloud::OSS