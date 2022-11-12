#include <include/http_handler.hpp>
#include <lib/clue.hpp>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

HttpHandler::HttpHandler(const std::string &server_url,
                         const std::string &auth_token)
    : m_server_url{server_url}, m_auth_token{auth_token}, m_client{http_client(
                                                              U(server_url))} {}

HttpHandler::~HttpHandler() {}

const http_response HttpHandler::httpGet(const std::string &api_path) {
  http_response response;
  LOG_DEBUG("HttpHandler - GET - " << m_server_url << api_path);
  auto requestJson = m_client.request(methods::GET, api_path)
                         .then([&response](http_response http_response) {
                           response = http_response;
                         });
  requestJson.wait();
  return response;
}

const http_response HttpHandler::httpPut(const std::string &api_path,
                                         const web::json::value &json) {
  http_response response;
  LOG_DEBUG("HttpHandler - PUT - " << m_server_url << api_path);
  auto requestJson = m_client.request(methods::PUT, api_path, json)
                         .then([&response](http_response http_response) {
                           response = http_response;
                         });
  requestJson.wait();
  return response;
}

const http_response HttpHandler::httpPost(const std::string &api_path,
                                          const web::json::value &json) {
  http_response response;
  LOG_DEBUG("HttpHandler - POST - " << m_server_url << api_path);
  auto requestJson = m_client.request(methods::POST, api_path, json)
                         .then([&response](http_response http_response) {
                           response = http_response;
                         });
  requestJson.wait();
  return response;
}

const http_response HttpHandler::httpDelete(const std::string &api_path) {
  http_response response;
  LOG_DEBUG("HttpHandler - DELETE - " << m_server_url << api_path);
  auto requestJson = m_client.request(methods::DEL, api_path)
                         .then([&response](http_response http_response) {
                           response = http_response;
                         });
  requestJson.wait();
  return response;
}

std::shared_ptr<HttpHandlerInterface>
HttpHandlerInterface::get(const std::string &url, const std::string &token) {
  return std::make_shared<HttpHandler>(url, token);
}
