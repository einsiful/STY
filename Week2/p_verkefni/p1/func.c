int a = 42;
const int b = 1;

int func(int *parg) {
  static int s = 0;
  int r;
  if (s == 0) {
    r = *parg + a;
    a++;
  } else {
    r = *parg + b;
    s = 1;
  }
  return r;
}
