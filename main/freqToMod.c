int freqToMod(int freq) {
    float x = 48000000/(128*(float)freq);
    return (int)x;
}
