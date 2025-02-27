extern scribor(char text);

int foo(int a, int b) {
    a = (2 + 10) * b;
    foo(6, a);
    return a;
}

int main() {
    foo(1, 2);
}
