#pragma once
#ifndef LightWSClient_include_WebSocketClient_H_
#define LightWSClient_include_WebSocketClient_H_
#include <functional>
#include <optional>
#include <string>
#include <vector>

class WebSocketClient {
public:
    template <typename T>
    using Callback = std::function<void(WebSocketClient&, T)>;

    enum class Status { Open, Closing, Closed };

    WebSocketClient();
    WebSocketClient(const WebSocketClient&)            = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;
    ~WebSocketClient();

    /**
     * @brief Get status.
     * @return WebSocketClient::Status
     */
    Status GetStatus() const { return status; }

    /**
     * @brief Connect by websocket uri.
     * @param url e.g "ws://hostname[:port][/path/path][?query=value]"
     */
    void Connect(const std::string& url, std::optional<std::string> const& token = {});

    /**
     * @brief Connect by hostname, port and path
     * @param hostname e.g "localhost" or "example.com"
     * @param port e.g 80
     * @param path e.g "/chat?id=1234"
     */
    void Connect(
        const std::string&                hostname,
        int                               port,
        const std::string&                path  = "/",
        std::optional<std::string> const& token = {}
    );

    /**
     * @brief Abort the connection and close tcp socket.
     */
    void Shutdown();

    /**
     * @brief After receving text data, the callback function will be called.
     * @param callback Callback funtion.
     */
    void OnTextReceived(Callback<std::string> callback);

    /**
     * @brief After receving binary data, the callback function will be called.
     * @param callback Callback funtion.
     */
    void OnBinaryReceived(Callback<std::vector<uint8_t>> callback);

    /**
     * @brief If an error occurs during the receiving process (such as a wrong frame),
              this function will be called.
     * @param callback Callback funtion.
    */
    void OnError(Callback<std::string> callback);

    /**
     * @brief If tcp connection aborted or received close frame,
     *		  the callback function will be called.
     * @param callback Callback funtion.
     */
    void OnLostConnection(Callback<int> callback);

    /**
     * @brief Send text data.
     * @param text Text data.
     */
    void SendText(const std::string& text);

    /**
     * @brief Send binary data.
     * @param data Binary data.
     * @param length Length of the binary data.
     */
    void SendBinary(const char* data, size_t length);

    /**
     * @brief Send binary data.
     * @param data Binary data.
     * @param length Length of the binary data.
     */
    void SendBinary(const uint8_t* data, size_t length);

    /**
     * @brief Send ping frame.
     */
    void Ping();

    /**
     * @brief Send pong frame.
     */
    void Pong();

    /**
     * @brief Send pong frame.
     * @param ping data
     */
    void Pong(const std::vector<uint8_t>& data);

    /**
     * @brief Send a close frame.
              If websocket server response a close frame,
              will close tcp socket.
    */
    void Close();

private:
    void                           RecvLoop();
    Callback<std::string>          TextReceivedCallback;
    Callback<std::vector<uint8_t>> BinaryReceivedCallback;
    Callback<std::string>          ErrorCallback;
    Callback<int>                  LostConnectionCallback;
    Status                         status;
    // avoid including implementation-related headers.
    struct pimpl;
    pimpl* PrivateMembers;
};

#endif // !LightWSClient_include_WebSocketClient_H_