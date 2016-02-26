var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){

  console.log('socket connected');
  
  socket.on('input_message', function(msg){
    console.log('incoming input_message was: ' + msg);
     io.emit('broadcast_message', msg);
  });

});

http.listen(3000, function(){
  console.log('listening on localhost:3000');
});
