
#include "null0.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s cart[.null0|.wasm|.zip]\n", argv[0]);
    return 1;
  }

  Null0CartData initcart = {
      .filename = argv[1]};

  Null0CartData* cart = null_cart_to_wasm(initcart);
  if (cart == NULL) {
    fprintf(stderr, "Could not load cart.\n");
    return 1;
  }

  bool s = null0_load(cart);

  free(cart->bytes);

  if (!s) {
    fprintf(stderr, "Could not initialize cart.\n");
    return 1;
  }

  return 0;
}