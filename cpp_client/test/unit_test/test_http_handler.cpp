#include <gtest/gtest.h>
#include <include/http_handler.hpp>

#define SERVER_URL "https://httpbin.org"
#define TOKEN "none"
#define DATA "data"
#define FIELD_KEY "field1"

class HttpHandlerFixture : public ::testing::Test {
protected:
  std::shared_ptr<HttpHandlerInterface> http_handler;
  web::json::value fixture_json;
  virtual void SetUp() {
    this->http_handler =
        std::move(HttpHandlerInterface::get(SERVER_URL, TOKEN));
    fixture_json[FIELD_KEY] = web::json::value::string(DATA);
  }
};

TEST_F(HttpHandlerFixture, httpGet) {
  auto response = http_handler->httpGet("/json");
  auto json_resp = response.extract_json().get();
  EXPECT_EQ(response.status_code(), HttpHandler::OK);
  EXPECT_TRUE(json_resp["slideshow"]["author"].as_string() ==
              (std::string) "Yours Truly");
}

TEST_F(HttpHandlerFixture, httpPost) {
  auto response = http_handler->httpPost("/post", fixture_json);
  auto json = response.extract_json().get();
  EXPECT_EQ(response.status_code(), HttpHandler::OK);
  EXPECT_TRUE(json["json"][FIELD_KEY].as_string() == (std::string)DATA);
}

TEST_F(HttpHandlerFixture, httpPut) {
  auto response = http_handler->httpPut("/put", fixture_json);
  auto json = response.extract_json().get();
  EXPECT_EQ(response.status_code(), HttpHandler::OK);
  EXPECT_TRUE(json["json"][FIELD_KEY].as_string() == (std::string)DATA);
}

TEST_F(HttpHandlerFixture, httpDelete) {
  auto response = http_handler->httpDelete("/delete");
  auto json = response.extract_json().get();
  EXPECT_EQ(response.status_code(), HttpHandler::OK);
  EXPECT_TRUE(json["json"].is_null());
}
