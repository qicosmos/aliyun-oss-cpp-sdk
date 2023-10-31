#pragma once

namespace AlibabaCloud::OSS {
class CoroHttpClient : public HttpClient {
public:
  CoroHttpClient() = default;
  virtual std::shared_ptr<HttpResponse>
  makeRequest(const std::shared_ptr<HttpRequest> &request) override {
    return async_simple::coro::syncAwait(makeRequestAsync(request));
  }

  async_simple::coro::Lazy<std::shared_ptr<HttpResponse>>
  makeRequestAsync(const std::shared_ptr<HttpRequest> &request) override {
    coro_http::coro_http_client client{};

    auto url = request->url().toString();
    auto method = request->method();

    cinatra::uri_t u;
    u.parse_from(url.data());

    std::unordered_map<std::string, std::string> headers;
    auto &req_headers = request->Headers();
    for (auto &[k, v] : req_headers) {
      // std::cout << k << ":" << v << "\n";
      headers.emplace(k, v);
    }

    headers.emplace("Host", u.get_host());

    auto body = request->Body();
    bool has_content = (body != nullptr);

    std::string content;
    coro_http::resp_data result;
    switch (method) {
    case Http::Method::Head:
      result = co_await client.async_head(url, std::move(headers));
      break;
    case Http::Method::Get:
      result = co_await client.async_get(url, std::move(headers));
      break;
    case Http::Method::Put:
      result = co_await client.async_upload_chunked(
          url, cinatra::PUT, request->Body(), cinatra::req_content_type::none,
          std::move(headers));
      break;
    case Http::Method::Post:
      if (has_content) {
        body->seekg(0, std::ios::end);
        int size = body->tellg();
        content.resize(size);
        body->seekg(0, std::ios::end);
        body->seekg(0, std::ios::beg);
        body->read(content.data(), size);
      }

      result = co_await client.async_post(url, std::move(content),
          cinatra::req_content_type::none,
          std::move(headers));
      break;

    default:
      std::cout << "not implement\n";
      break;
    }

    std::cout << result.status << "\n";
    std::cout << result.resp_body << "\n";

    auto response = std::make_shared<HttpResponse>(request);
    response->setStatusCode(result.status);
    if (result.net_err) {
      response->setStatusMsg(result.net_err.message().data());
    }
    if (!result.resp_body.empty()) {
      auto ss = std::make_shared<std::stringstream>();
      ss->write(result.resp_body.data(), result.resp_body.size());
      response->addBody(ss);
    }

    co_return response;
  }

private:
  ClientConfiguration conf_;
};
} // namespace AlibabaCloud::OSS