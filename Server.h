#include <iostream>
#include <unordered_map>
#include <functional>
#include <boost/asio.hpp>
#define BUFFER_SIZE 4096
#define PORT 9633
#define WEB_ROOT "."
#pragma once

using namespace boost::asio;
using namespace boost::asio::ip;
// 路由处理函数类型
using Handler = std::function<std::string(const std::string&)>;
class Server
{
	public:
		Server(io_context& io, short port) : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
			start_accept();
		};
		virtual ~Server() {};
		void add_route(const std::string& path, Handler handler) {
			routes_[path] = handler;
		}
	private:
		void start_accept();
		void handle_connection(std::shared_ptr<tcp::socket> socket);
		tcp::acceptor acceptor_;
		std::unordered_map<std::string, Handler> routes_;
};
