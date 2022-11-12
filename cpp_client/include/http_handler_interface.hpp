#pragma once
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <memory>

class HttpHandlerInterface {
public:
  enum HttpStatusCode {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404
  };

  virtual ~HttpHandlerInterface(){};
  virtual const web::http::http_response
  httpGet(const std::string &api_path) = 0;
  virtual const web::http::http_response
  httpPut(const std::string &api_path, const web::json::value &json) = 0;
  virtual const web::http::http_response
  httpPost(const std::string &api_path, const web::json::value &json) = 0;
  virtual const web::http::http_response
  httpDelete(const std::string &api_path) = 0;
  static std::shared_ptr<HttpHandlerInterface> get(const std::string &url,
                                                   const std::string &token);
};
