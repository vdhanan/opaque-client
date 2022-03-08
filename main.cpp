#include <iostream>
#include <string>
#include <curl/curl.h>
#include "opaque_ke_cxx.h"
#include <unordered_map>
#include <vector>

using namespace std;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  rust::Vec<uint8_t> *p = static_cast<rust::Vec<uint8_t> *>(contents);
  for (uint8_t c : *p)
  {
    ((rust::Vec<uint8_t> *)userp)->push_back(c);
  }
  return size * nmemb;
}

int client_register(string password)
{

  MessageState messageState;
  // try to get client registration message and state from opaque-ke-cxx
  try
  {
    messageState = client_register_cxx(password);
  }
  catch (const exception &e)
  {
    cout << "client registration start failure" << e.what();
    return 1;
  }

  // write registration request bytes to file so it can be transmitted to server

  FILE *pFile;
  pFile = fopen("/tmp/client-registration-request", "w");
  for (uint8_t it : messageState.message)
  {
    fputc(it, pFile);
  }
  fclose(pFile);

  // set up curl

  CURL *curl;
  CURLcode res;
  rust::Vec<uint8_t> startReadBuffer;

  curl = curl_easy_init();

  // send curl request to server with registration request bytes
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, "localhost:7878/register/start");
    // curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    // curl_easy_setopt(curl, CURLOPT_READDATA, pFile);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &startReadBuffer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      cout << "curl error!";
      return 1;
    }
  }

  // try to finish client registration with client_register_finish_cxx
  rust::Vec<uint8_t> registration_finish_bytes;
  try
  {
    cout << "messageState.state: ";
    for (auto i : messageState.state) {
      cout << i;
    }
    cout << endl;
    registration_finish_bytes = client_register_finish_cxx(messageState.state, startReadBuffer);
  }
  catch (const exception &e)
  {
    cout << "client registration finish failure" << e.what();
    return 1;
  }

  // write registration finish request bytes to file
  pFile = fopen("/tmp/client-registration-finish-request", "w");
  for (uint8_t it: registration_finish_bytes)
  {
    fputc(it, pFile);
  }
  fclose(pFile);

  // send curl request to server with registration finish request bytes
  rust::Vec<uint8_t> finishReadBuffer;
  curl_easy_setopt(curl, CURLOPT_URL, "localhost:7878/register/finish");
  // curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  // curl_easy_setopt(curl, CURLOPT_READDATA, pFile);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &finishReadBuffer);
  res = curl_easy_perform(curl);
  if (res != CURLE_OK)
  {
    return 1;
  }
  
  curl_easy_cleanup(curl);
  return 0;
}

  // curl = curl_easy_init();
  // if(curl) {
  //   curl_easy_setopt(curl, CURLOPT_URL, "localhost:7878/register/start");
  //   curl_easy_setopet(curl, )
  //   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  //   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
  //   res = curl_easy_perform(curl);
  //   curl_easy_cleanup(curl);

  //   cout << readBuffer << endl;
  // }
  // curl_easy_cleanup(curl);
  // return 0;
  // }

int main(void)
{
  // unordered_map<string, std::vector<uint8_t>> umap;
  // while (true)
  // {
    //  cout << "\n Currently registered usernames: \n" << endl;
    //  for (auto it : umap)
    //  {
    //    cout << it.first << "\n" << endl;
    //  }
  // }
  string password = "hunter2";
  int reg_result = client_register(password);
  return reg_result;
}