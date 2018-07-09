// Include standard headers
#include "Main.h"
// Jordan Machalek
// Section 1
// E06
int main(void)
{
	Application* pApp = new Application();
	pApp->Init("", RES_C_1280x720_16x9_HD, false, false);
	pApp->Run();
	SafeDelete(pApp);
	return 0;
}