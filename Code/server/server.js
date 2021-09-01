var MS = require("mongoskin");
var express = require("express");
var app = express();
var bodyParser = require('body-parser');
var errorHandler = require('errorhandler');
var methodOverride = require('method-override');
var hostname = process.env.HOSTNAME || 'localhost';
var port = 1234;
var VALUEx = "";
var VALUEtime = 0;

var db = MS.db("mongodb://localhost:27017/data");

app.get("/", function (req, res) {
    res.redirect("/index.html");
});

app.get("/getLatest", function (req, res) {
  db.collection("data").find({}).sort({time:-1}).limit(10).toArray(function(err, result){
    res.send(JSON.stringify(result));
  });
});

app.get("/getData", function (req, res) {
  var from = parseInt(req.query.from);
  var to = parseInt(req.query.to);
  db.collection("data").find({time:{$gt:from, $lt:to}}).sort({time:-1}).toArray(function(err, result){
    res.send(JSON.stringify(result));
  });
});

app.get("/getValue", function (req, res) {
  //res.writeHead(200, {'Content-Type': 'text/plain'});
  res.send(VALUEtime + ":" + VALUEx.toString());
});

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


app.use(methodOverride());
app.use(bodyParser());
app.use(express.static(__dirname + '/public'));
app.use(errorHandler());

console.log("Simple static server listening at http://" + hostname + ":" + port);
app.listen(port);
