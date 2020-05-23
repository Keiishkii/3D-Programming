#ifndef INPUT_HANDLER
#define INPUT_HANDLER

struct Inputs
{
	bool quit = false;

	bool key_W = false;
	bool key_A = false;
	bool key_S = false;
	bool key_D = false;

	bool key_SPACE = false;
	bool key_LSHIFT = false;

	int xrel = 0;
	int yrel = 0;

	float mouseWheelY = 0;
};

class InputHandler
{
private:
	static Inputs inputs;
public:
	static void processInputs();
	static Inputs* getInputs();
};

#endif // !INPUT_HANDLER
/*

*/