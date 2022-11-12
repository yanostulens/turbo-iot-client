#pragma once

#include <include/http_handler_interface.hpp>

class HttpHandler : public HttpHandlerInterface {
public:
  HttpHandler(const std::string &server_url, const std::string &auth_token);
  ~HttpHandler();
  const web::http::http_response httpGet(const std::string &api_path) override;
  const web::http::http_response httpPut(const std::string &api_path,
                                         const web::json::value &json) override;
  const web::http::http_response
  httpPost(const std::string &api_path, const web::json::value &json) override;
  const web::http::http_response
  httpDelete(const std::string &api_path) override;

private:
  const std::string m_server_url;
  const std::string m_auth_token;
  web::http::client::http_client m_client;
};
