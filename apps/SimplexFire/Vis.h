static byte heat[NUM_LEDS];

void Vis(LedRopeTCL* rope) {
  rope->FillColor(Color3i::Green());
  rope->Draw();
}
