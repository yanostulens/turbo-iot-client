#include <lib/clue.hpp>
#include <test/include/mock_http_handler.hpp>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

MockHttpHandler::MockHttpHandler(const std::string &server_url,
                                 const std::string &auth_token)
    : m_server_url{server_url}, m_auth_token{auth_token}, m_client{NULL} {}

MockHttpHandler::~MockHttpHandler() {}

const http_response MockHttpHandler::httpGet(const std::string &api_path) {
  http_response response;
  LOG_DEBUG("MockHttpHandler - GET - " << m_server_url << api_path);
  response.set_status_code((web::http::status_code)m_mock_status_code);
  response.set_body(m_mock_json);
  return response;
}

const http_response MockHttpHandler::httpPut(const std::string &api_path,
                                             const web::json::value &) {
  http_response response;
  LOG_DEBUG("MockHttpHandler - PUT - " << m_server_url << api_path);
  response.set_status_code((web::http::status_code)m_mock_status_code);
  response.set_body(m_mock_json);
  return response;
}

const http_response MockHttpHandler::httpPost(const std::string &api_path,
                                              const web::json::value &) {
  http_response response;
  LOG_DEBUG("MockHttpHandler - POST - " << m_server_url << api_path);
  response.set_status_code((web::http::status_code)m_mock_status_code);
  response.set_body(m_mock_json);
  return response;
}

const http_response MockHttpHandler::httpDelete(const std::string &api_path) {
  http_response response;
  LOG_DEBUG("MockHttpHandler - DELETE - " << m_server_url << api_path);
  response.set_status_code((web::http::status_code)m_mock_status_code);
  response.set_body(m_mock_json);
  return response;
}

std::shared_ptr<HttpHandlerInterface>
HttpHandlerInterface::get(const std::string &url, const std::string &token) {
  return std::make_shared<MockHttpHandler>(url, token);
}
