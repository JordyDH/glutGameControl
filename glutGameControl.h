void glutGameMainLoop();
void glutGameRenderCamera();
void glutGameInitCamera(double x, double y, double z);
void glutGameSetRenderScene(void (*fnc_p)());
void glutGameRenderScene();
void glutGameRescale(GLint n_w, GLint n_h);
void glutGameRender(int systick_old);
double glutGameGetFPS();
void glutGameMouseFunction(int button, int state, int x, int y);
void glutGameMouseMove(int x, int y);
void glutGameKeyboardInit();
void glutGameKeyboardPressed(int key, int x, int y);
void glutGameKeyboardReleased(int key, int x, int y);
void glutGameMoveCamera(int key);
void glutGameRotateCamera(double dxa, double dza);
void glutGameRenderLocalAxis();
