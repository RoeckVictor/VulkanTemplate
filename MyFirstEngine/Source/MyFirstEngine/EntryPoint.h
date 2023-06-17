#pragma once

#ifdef MFE_PLATFORM_WINDOWS

extern MyFirstEngine::Application* MyFirstEngine::CreateApplication();

int main(int argc, char** argv)
{
	MyFirstEngine::Log::Init();
	MFE_CORE_WARN("Initialized Log!");
	int a = 5;
	MFE_INFO("Hello! Var={0}", a);

	auto app = MyFirstEngine::CreateApplication();
	app->Run();
	return 0;
}
#endif
