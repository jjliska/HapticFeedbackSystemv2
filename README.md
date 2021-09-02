# Haptic Feedback System v2

<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/Concept.png" width="600" height="300">

#### Table of Contents  
&ensp;[Project Description](#Project-Description)  
&ensp;[Demo Video](#Demo-Video)  
&ensp;[Improvements](#Improvements)  
&ensp;[Explanations](#Explanations)  
&ensp;[References](#References)  

## Project Description  
&ensp; Haptic Feedback System v2 is a continuation of the idea for a haptic feedback system that allows users or information from touch sensory to travel long distance. This is incredibly useful for particular applications of isolation and feedback systems for games or research. One potential for this project is the ability to "feel" another user touching or playing with a sort of membrane that allows two users to interact across the membrane, sensing each others hands and motion on said membrane.

## Demo Video
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/iuzkEGYAWhc/0.jpg)](https://www.youtube.com/watch?v=iuzkEGYAWhc)

## Images
&ensp;<sup>[Back to Top](#Haptic-Feedback-System-v2)</sup>  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210225_181757.jpg" width="320" height="480">  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210225_181802.jpg" width="320" height="480">  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210225_182047.jpg" width="320" height="480">  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210225_213826.jpg" width="320" height="480">  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210225_214212.jpg" width="320" height="480">  
<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/20210226_025133.jpg" width="320" height="480">  

## Improvements
&ensp;<sup>[Back to Top](#Haptic-Feedback-System-v2)</sup>  
&ensp;Unlike the previous iteration that had actually had a haptic feedback system installed directly into the object. this iteration has no literal feedback system. Instead it directly stores data as a json on a live mongodb server. This data is stored and then sent to a live webservers hosting a simple html script that shows the user a rough pixel approximation of where their hand is and the amount of pressure being displayed through color and the console.
&ensp;This could easily be made into two products one of which sends and the other of which receives this would then make the product tangible in the interaction of the two parts.
This step could then be taken one step further and the two pieces integrated into each other and then multiplied into two independent systems that communicate with the webserver and grab the others data allowing the two interfaces to interact with eachother.  
&ensp;The design for this concept would use some sort of piezo-electric material, variable solenoid, or electrically expanding fluid to stimulate the sensation of pressure given a specific voltage. The concept idea that was most feasible was to use some sort of air pressure in a small version. This would allow for the price of the prototype to stay low while still allowing for the dexterity of pressure sensitivity in a small space, as only the tube needs to run in between the piece and the electrical grid. This would be printed in "balloons" of some sort of TPU or silicone to allow for expansion as well as a semi rigid grid that the "balloons" sit in.  


## Explanations
&ensp;<sup>[Back to Top](#Haptic-Feedback-System-v2)</sup>  
### Construction  
&ensp;The construction of the piece is made of a PLA backplate, which then has a copper tape and clear acrylic tape placed in between these pieces to attempt to hold down the velostat placed on top of it. This is intended to stop the copper and velostat from moving when pressure is applied to it. The top plate is then made of a TPU material that allows for flexibility and pressure to more easily go directly to the copper bands that are being pressed against.

### Data Gathering
&ensp;Data is obtained from polling the connections between the two bands of copper. One band is running vertically while the other is perpendicular to this, aka horizontal. When pressure is applied to the material the velostat in between the two bands is compressed allowing electricity to more easily flow through it and reducing the amount of resistance the material applies.

<details><summary>C/C++ Script</summary>
<p>

```c
int preI = 0, preJ = 0;

void readMatrix(bool startup){
  int analogIn;
  for(int i=0;i<9;i++){
    digitalWrite(rows[preI], LOW);
    digitalWrite(rows[i], HIGH);
    preI = i;
    for(int j=0;j<6;j++){
      digitalWrite(cols[preJ], LOW);
      digitalWrite(cols[j], HIGH);
      preJ = j;
      analogIn = analogRead(rows[i]);
      if(!startup){
        if((analogIn - calibrationMatrix[i][j])<=0) pressureMatrix[i][j] = 0.0;
        else pressureMatrix[i][j] = float(analogIn);
      }
      else calibrationMatrix[i][j] = calibrationMatrix[i][j] + (float(analogIn)/sampleNum);
    }
  }
}
```
                                                   
&ensp;[From HapticFeedback.ino](https://https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/HapticFeedback/HapticFeedback.ino)

</p>
</details>

### Data Storage
&ensp;The data from the matrix to a web server that stores the data into a mongoDB database. This information is displayed on the website that is also used to collect data so that the user can view the live output of the matrix they are interacting with.

<details><summary>C/C++ Script</summary>
<p>

```c
/*  void httpGETRequest()
 *    sets up communication with the AWS server
 */
String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

/*  void toUrl()
 *    takes the array data taken from the pressure sensor and parses it into a readable format for the AWS website to handle
 */
String toUrl(){
  String url = serverName;
  for(int i=0;i<9;i++){
    for(int j=0;j<6;j++){
      if(j<5) url = url + pressureMatrix[i][j] + " ";
      else url = url + pressureMatrix[i][j];
    }
    if(i<8) url = url + ",";
  }
  return url;
}
```  
  
&ensp;[From HapticFeedback.ino](https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/HapticFeedback/HapticFeedback.ino)

</p>
</details>

<details><summary>JavaScript</summary>
<p>
  
```JavaScript
var db = MS.db("mongodb://localhost:27017/data");

...

app.get("/sendData", function (req, res) {
  VALUEx = req.query.x;
  VALUEtime = new Date().getTime();
	var dataObj = {
		x: VALUEx,
		time: VALUEtime
	}
	db.collection("data").insert(dataObj, function(err,result){
		console.log("added data: " + JSON.stringify(dataObj));
	});
  res.send(VALUEtime.toString());
});
```

&ensp;[From server.js](https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/server/server.js)

</p>
</details>

## References
&ensp;<sup>[Back to Top](#Haptic-Feedback-System-v2)</sup>  
<a href="https://www.nanomotion.com/nanomotion-technology/piezoelectric-effect/">Piezo-Electric effect and the nanomotion produced in a piezo-electric system</a>
