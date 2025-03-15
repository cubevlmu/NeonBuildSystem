#include "nbs/NBSCtx.hpp"

int main(int argc, char** argv) {
    nbs::NBSCtx ctx {argc, argv};
    ctx.globalInit();
    return ctx.runScript();
}