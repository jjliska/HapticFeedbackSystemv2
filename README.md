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
&ensp;The data is stored initially in a 2d list object on the ESP-32. This is then passed to a stringify function toURL() which formats the list into a string that can be passed to the web server and then later stored in a mongoDB database. 

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

### Data Display
&ensp;The data is displayed in many various ways. On the ESP-32 through the serial communication line the data is output as the list formatted in a more visually pleasing way. This can easily be turned off so that the ESP-32 can run more efficiently, however it was done during the initial stage of prototyping and was never turned off.

<details><summary>C/C++ Script</summary>
<p>

```c
void printMatrix(bool startup){
  Serial.print("Url: ");Serial.println(toUrl());
  Serial.println("--------------------------------------------------------");
  for(int i=0;i<9;i++){
    for(int j=0;j<6;j++){
      Serial.print("[ ");
      if(!startup) Serial.print(pressureMatrix[i][j]);
      else Serial.print(calibrationMatrix[i][j]);
      Serial.print(" ]");
    }
    Serial.println();
  }
  Serial.println("--------------------------------------------------------");
}
```  
  
&ensp;[From HapticFeedback.ino](https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/HapticFeedback/HapticFeedback.ino)

</p>
</details>

<details><summary>JavaScript</summary>
<p>
  
&ensp;The data is also visually displayed to client via a neat little website that allows the user to view the pressure on the device at a low refresh rate, roughly ten times a seocnd, for a frame rate of 10. This can easily be increased but would require data to roll over or not be stored at all and just passed to the server and held until it was retrieved by the other device.
	
```JavaScript
function getDataFromServer(){
	var url = "./getValue"
	var callback = function(data){
		console.log(data, typeof data);
		var args = data.split(":");
		var time = args[0];
		var rows = args[1].split("_");
		var table = new Array(rows.length);
		var size = 75;

		for (var i=0;i<rows.length; i++){
		  table[i] = rows[i].split("-");
			for(var j=0;j<table[i].length;j++){
				table[i][j] = parseFloat(table[i][j]);
			}
		}

		var canvas = document.getElementById('canvas');
		var context = canvas.getContext('2d');
		if (canvas.getContext){
			for(var i=0;i<table.length;i++){
				var inverseJ = 0;
				for(var j=0;j<table[i].length;j++){
					if(j <= 0) inverseJ = table[i].length-1;
					else inverseJ--;
					if(table[i][inverseJ] <= 0){
						context.fillStyle = 'rgb(0,0,255)';
					}
					else if(table[i][inverseJ] >= 1000.0){
						context.fillStyle = 'rgb(0,255,0)';
					}
					else{
						context.fillStyle = 'rgb(0,'
						+ parseInt((table[i][inverseJ]/500.0)*255.0) + ','
						+ parseInt(255.0-((table[i][inverseJ]/500.0)*255.0)) + ')';
					}
			context.fillRect((i*size),(j*size),size,size);
				}
			}
		}

		console.table(table);
		var output = "<p>" + new Date(parseInt(time)).toString() + "</p>";

		document.getElementById("time").innerHTML = output;
	}
	loadFile(url, callback);
}
```

&ensp;[From index.html](https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/server/public/index.html)

</p>
</details>

&ensp;Lastly, the data can be interpolated by other informatics programs such as a simple python program I wrote to read the entirety of the database and display it as a function of average pressure on the system over time. This could be done for individual spots to determine where the user is placing the most pressure on and other statistical data needed from the system.

<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/Graph.png" width="480" height="320">

<img src="https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Media/Parsed_Database.png" width="600" height="320">

<details><summary>Python</summary>
<p>
  	
```python
for data in obj:
    averagePressure = 0
    dataString = data.get("x")
    if data.get("x") != "[object Undefined]":
        count+=1
        tempArray = dataString.split("_")
        for i in range(0,len(tempArray)):
            tempArray[i] = tempArray[i].replace(" ","-").split("-")
            for j in range(0, len(tempArray[i])):
                try:
                    averagePressure+=float(tempArray[i][j])
                except:
                    break
        averagePressure=averagePressure/float(len(tempArray)*len(tempArray[0]))
        avPressureList.append(averagePressure)
        totPressure+=averagePressure

totPressure=totPressure/float(count)
print("Total number of valid objects: %i" %count)
print("Average pressure across the grid: %f" %totPressure)

#Showing the graph
plt.plot(range(0,count),avPressureList)
```

&ensp;[From JsonPY.py](https://github.com/jjliska/HapticFeedbackSystemv2/blob/main/Code/JsonPY.py)

</p>
</details>

## References
&ensp;<sup>[Back to Top](#Haptic-Feedback-System-v2)</sup>  
<a href="https://www.nanomotion.com/nanomotion-technology/piezoelectric-effect/">Piezo-Electric effect and the nanomotion produced in a piezo-electric system</a>
