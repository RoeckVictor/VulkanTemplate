#pragma once

#ifdef MFE_PLATFORM_WINDOWS

extern MyFirstEngine::Application* MyFirstEngine::CreateApplication();

int main(int argc, char** argv)
{
	MyFirstEngine::Log::Init();

	MFE_PROFILE_BEGIN_SESSION("Startup", "Logging/Profile/MyFirstEngineProfile-Startup.json");
	auto app = MyFirstEngine::CreateApplication();
	MFE_PROFILE_END_SESSION();

	MFE_PROFILE_BEGIN_SESSION("Runtime", "Logging/Profile/MyFirstEngineProfile-Runtime.json");
	app->Run();
	MFE_PROFILE_END_SESSION();

	MFE_PROFILE_BEGIN_SESSION("Shutdown", "Logging/Profile/MyFirstEngineProfile-Shutdown.json");
	delete app;
	MFE_PROFILE_END_SESSION();

	return 0;
}
#endif
