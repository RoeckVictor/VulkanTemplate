#pragma once

#ifdef MFE_PLATFORM_WINDOWS

extern MyFirstEngine::Application* MyFirstEngine::CreateApplication();

int main(int argc, char** argv)
{
	MyFirstEngine::Log::Init();

	auto app = MyFirstEngine::CreateApplication();
	app->Run();
	return 0;
}
#endif
