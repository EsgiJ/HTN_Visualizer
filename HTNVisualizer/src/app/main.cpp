#include "htn/app/application.h"

int main() {
	HTN::App::Application app;

	if (!app.Initialize()) {
		return EXIT_FAILURE;
	}

	app.Run();
	app.Shutdown();

	return EXIT_SUCCESS;
}