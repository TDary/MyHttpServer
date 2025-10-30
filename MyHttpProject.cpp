// MyHttpProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"
#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
int main()
{
	try
	{
		io_context io;
		Server server(io, PORT);
		// 注册路由
		server.add_route("/", [](const std::string&) {
			return "<h1>Welcome to Boost HTTP Server!</h1>"
				"<p>Try <a href='/hello'>/hello</a> or <a href='/time'>/time</a></p>";
		});

		server.add_route("/hello", [](const std::string&) {
			return "<h1>Hello from Boost.Asio!</h1>";
		});

		server.add_route("/time", [](const std::string&) {
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			return "<h1>Server Time: " + std::string(std::ctime(&time)) + "</h1>";
		});

		server.add_route("/post", [](const std::string& body) {
			return "<h1>Received POST data:</h1><pre>" + body + "</pre>";
		});

		std::cout << "HTTP server running on port " << PORT << std::endl;

		io.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Server exception: " << e.what() << std::endl;
	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
