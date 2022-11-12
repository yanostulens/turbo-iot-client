#pragma once

#include <include/http_handler_interface.hpp>

class MockHttpHandler : public HttpHandlerInterface {
public:
  MockHttpHandler(const std::string &server_url, const std::string &auth_token);
  ~MockHttpHandler();
  const web::http::http_response httpGet(const std::string &api_path) override;
  const web::http::http_response httpPut(const std::string &api_path,
                                         const web::json::value &json) override;
  const web::http::http_response
  httpPost(const std::string &api_path, const web::json::value &json) override;
  const web::http::http_response
  httpDelete(const std::string &api_path) override;

  void setStatusCode(const HttpHandlerInterface::HttpStatusCode &status) {
    this->m_mock_status_code = status;
  }
  void setJsonResponse(const web::json::value &json) {
    this->m_mock_json = json;
  }

private:
  const std::string m_server_url;
  const std::string m_auth_token;
  web::http::client::http_client m_client;
  web::json::value m_mock_json;
  HttpHandlerInterface::HttpStatusCode m_mock_status_code;
};
