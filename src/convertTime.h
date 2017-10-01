void convertTime(int Hour24, int *Hour12, int *AorP) {
  *AorP = 0;                              // set as am
  *Hour12 = Hour24;
  if (!Time24) {                          // 24 hour = false
    if (*Hour12 >= 12) {
      *Hour12 -= 12;                      // 1300 > 100
      *AorP = 1;                          // pm
    }
  }
}
