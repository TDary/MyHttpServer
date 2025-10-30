#include "Server.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

// HTTP 响应构建器
std::string build_response(const std::string& content,const std::string& content_type = "text/html",int status_code = 200) {
	std::string status_text = "OK";
	if (status_code == 404)	status_text = "Not Found";
	else if (status_code == 500)	status_text = "Internal Server Error";

	std::ostringstream response;
	response << "HTTP/1.1 " << std::to_string(status_code) << " " << status_text << "\r\n" << "Content-Type: " << content_type << "\r\n"
		<< "Content-Length: " << content.length() << "\r\n" << "Connection: close\r\n\r\n" << content;

	return response.str();
}

void Server::start_accept() {
	auto socket = std::make_shared<tcp::socket>(Server::acceptor_.get_executor());
	Server::acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
		if (!ec) {
			std::thread(&Server::handle_connection, this, socket).detach();
		}
		start_accept();
		});
}

void Server::handle_connection(std::shared_ptr<tcp::socket> socket) {
	try {
		// 读取请求
		streambuf buffers;
		read_until(*socket, buffers, "\r\n\r\n");

		// 解析请求行
		std::istream stream(&buffers);
		std::string method, path, http_version;
		stream >> method >> path >> http_version;

		// 跳过请求头
		std::string header;
		while (std::getline(stream, header) && header != "\r") {}

		// 读取请求体(POST)
		std::string body;
		if (method == "POST") {
			size_t content_length = 0;
			auto bufs = buffers.data();
			auto begin_it = boost::asio::buffers_begin(bufs);
			auto end_it = boost::asio::buffers_end(bufs);
			auto it = std::search(begin_it, end_it, "Content-Length: ", "Content-Length: " + 16);
			if (it!= end_it)	content_length = std::stoul(std::string(it + 16, std::find(it + 16, end_it, '\r')));

			// 读取剩余数据
			if (content_length > 0) {
				boost::system::error_code ec;
				read(*socket, buffers, transfer_exactly(content_length - (buffers.size() - (end_it - begin_it))), ec);
				body = std::string(begin_it, end_it);
			}
		}

		// 查找路由处理器
		std::string response;
		auto it = routes_.find(path);
		if (it != routes_.end())	response = build_response(it->second(body));
		else response = build_response("<h1>404 Not Found</h1>", "text/html", 404);

		// 发送响应
		write(*socket, buffer(response));
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		const std::string resp = build_response("<h1>500 Server Error</h1>", "text/html", 500);
		write(*socket, buffer(resp));
	}
}