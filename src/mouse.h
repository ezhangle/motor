#pragma once

typedef struct {
  void (*pressed)(void * userData, int x, int y, int button);
  void (*released)(void * userData, int x, int y, int button);
  void (*moved)(void * userData, int x, int y, int dx, int dy);
  void * userData;
} mouse_EventCallbacks;

void mouse_mousemoved(int x, int y);
void mouse_mousepressed(int x, int y, int button);
void mouse_mousereleased(int x, int y, int button);
int mouse_isDown(const char *str);
int mouse_isVisible(void);
void mouse_getPosition(int *x, int *y);
int mouse_getX(void);
int mouse_getY(void);
void mouse_setVisible(int b);
void mouse_setX(int x);
void mouse_setY(int y);
char const *mouse_getButtonName(int button);

void mouse_setEventCallbacks(mouse_EventCallbacks const *callbacks);
