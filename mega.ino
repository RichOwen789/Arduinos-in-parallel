int dataSize = 6;
byte numOnes = 0;
byte numWaves = 10;
byte data[] = {1, 0, 1, 0, 1, 0};
byte bestWaves[10];
byte tryData[6];

void setup() {

  //calc numOnes
  for(int i = 0; i < dataSize; i++){
    if(data[i] == 1){
      numOnes ++;
    }
  }
  
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  sendData(dataSize, numOnes, numWaves, data, 1);
  sendData(dataSize, numOnes, numWaves, data, 2);
}


void calcTryData() {
  
  //reset tryData
  for (int i = 0; i < dataSize; i ++) {
    tryData[i] = 0;
  }
  //crash the waves
  for (int i = 0; i < numWaves; i++) {
    if (bestWaves[i] > 0 && bestWaves[i] < dataSize+1) {
      int j = -1;
      do {
        j += bestWaves[i];
        tryData[j] ++;
      } while (j < dataSize);
    }
  }
  //revive 1's and 0's
  int ones = 0;
  int maxValue = 2*numWaves; //waves cannot make data >= than this
  while (ones < numOnes){
    int bigIndex = 0;
    for(int i = 0; i < dataSize; i++){
      if(tryData[i] != maxValue){
        bigIndex = i;
        break;
      }
    }
    for(int i = 0; i < dataSize; i++){
      if(tryData[i] >= tryData[bigIndex] && tryData[i] != maxValue){
        bigIndex = i;
      }
    }
    tryData[bigIndex] = maxValue; 
    ++ones;
  }
  for(int i = 0; i < dataSize; i++){
    tryData[i] == maxValue ? tryData[i] = 1 : tryData[i] = 0;
  }
  
}


boolean compareWaves(byte theseWaves[], byte thoseWaves[]) {
  //setup data
  byte theseData[dataSize];
  byte thoseData[dataSize];
  for (int i = 0; i < dataSize; i ++) {
    theseData[i] = 0;
    thoseData[i] = 0;
  }
  //crash the waves
  for (int i = 0; i < numWaves; i++) {
    if (theseWaves[i] > 0 && theseWaves[i] < dataSize+1) {
      int j = -1;
      do {
        j += theseWaves[i];
        theseData[j] ++;
      } while (j < dataSize);
    }
    if (thoseWaves[i] > 0 && thoseWaves[i] < dataSize+1) {
      int j = -1;
      do {
        j += thoseWaves[i];
        thoseData[j] ++;
      } while (j < dataSize);
    }
  }
  //revive 1's and 0's
  int theseOnes = 0;
  int thoseOnes = 0;
  byte maxValue = numWaves + 1; //waves cannot make data >= than this
  //these
  while (theseOnes < numOnes){
    int bigIndex = 0;
    for(int i = 0; i < dataSize; i++){
      if(theseData[i] >= theseData[bigIndex] && theseData[i] != maxValue){
        bigIndex = i;
      }
    }
    theseData[bigIndex] = maxValue; 
    theseOnes ++;
  }
  for(int i = 0; i < dataSize; i++){
    theseData[i] == maxValue ? theseData[i] = 1 : theseData[i] = 0;
  }
  //those
  while (thoseOnes < numOnes){
    int bigIndex = 0;
    for(int i = 0; i < dataSize; i++){
      if(thoseData[i] >= thoseData[bigIndex] && thoseData[i] != maxValue){
        bigIndex = i;
      }
    }
    thoseData[bigIndex] = maxValue; 
    thoseOnes ++;
  }
  for(int i = 0; i < dataSize; i++){
    thoseData[i] == maxValue ? thoseData[i] = 1 : thoseData[i] = 0;
  }

  // return theseData are better than thoseData
  int diff = 0;
  for(int i = 0; i < dataSize; i++) {
    if(thoseData[i] == data[i] && theseData[i] == data[i]) {
      diff += 0;
    }
    if(thoseData[i] == data[i] && theseData[i] != data[i]) {
      diff += -1;
    }
    if(thoseData[i] != data[i] && theseData[i] == data[i]) {
      diff += 1;
    }
  }
  return diff >= 0;
  
}


void sendData(byte dataSize, byte numOnes, byte numWaves, byte data[], int uno){
  byte info[] = {dataSize, numOnes, numWaves};
  if(uno == 1){
    Serial1.write(info, 3);
    Serial1.write(data, dataSize);
  }
  if(uno == 2){
    Serial2.write(info, 3);
    Serial2.write(data, dataSize);
  }
}


void sendWaves(byte waves[], byte uno){
  if(uno == 1){
    Serial1.write(bestWaves, numWaves);
  }
  if(uno == 2){
    Serial2.write(bestWaves, numWaves);
  }
}


void requestWaves(byte uno){
  if(uno == 1){
    Serial1.write(9);
  }
  if(uno == 2){
    Serial2.write(9);
  }
}


void receiveWaves(byte uno){
  
  byte waves[numWaves];
  if(uno == 1){
    //Receive the waves
    byte bytes_read = 0;
    while(bytes_read < numWaves){
      if(Serial1.available()){
        byte waveByte = Serial1.read();
        waves[bytes_read] = waveByte;
        bytes_read ++;
      }
    }
  }
  if(uno == 2){
    //Receive the waves
    byte bytes_read = 0;
    while(bytes_read < numWaves){
      if(Serial2.available()){
        byte waveByte = Serial2.read();
        waves[bytes_read] = waveByte;
        bytes_read ++;
      }
    }
  } 

  if(compareWaves(waves, bestWaves)){
    for(int i = 0; i < numWaves; i ++){
      bestWaves[i] = waves[i];
    }
  }
}


void loop() {

  sendWaves(bestWaves, 1);
  sendWaves(bestWaves, 2);
  
  requestWaves(1);
  receiveWaves(1);

  requestWaves(2);
  receiveWaves(2);

  if(Serial.available()){
    char show = Serial.read();
    if(show == 's'){
      Serial.println("\nBest Waves");
      for(int i = 0; i < numWaves; i++){
        Serial.print(bestWaves[i]);
      }
      Serial.println("\nOriginal data");
      for(int i = 0; i < dataSize; i++){
        Serial.print(data[i]);
      }
      calcTryData();
      Serial.println("\nGenerated data");
      for(int i = 0; i < dataSize; i++){
        Serial.print(tryData[i]);
      }
      Serial.println();
    }
  }
  
}
