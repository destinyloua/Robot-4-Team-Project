let currentCommand = null;
let currentDirection = null;

// select drive or sleep 
function selectCommand(command) {
    currentCommand = command;

    // highlight selected command
    document.querySelectorAll('.command-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    document.getElementById(`${command}Btn`).classList.add('active');

    // enable/disable inputs based on command
    const isDrive = command === 'drive';
    document.querySelectorAll('.direction-btn').forEach(btn => btn.disabled = !isDrive);
    document.getElementById('duration').disabled = !isDrive;
    document.getElementById('speed').disabled = !isDrive;
}

// only enabled if drive is selected 
function selectDirection(direction) {
    currentDirection = direction;

    // highlight selected direction
    document.querySelectorAll('.direction-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    document.getElementById(`${direction}Btn`).classList.add('active');
}

// only execute when user presses GO
function executeCommand() {
    let message = "";

    if (!currentCommand) {
        message = "Please select a command (Sleep or Drive).";
    } else if (currentCommand === "drive") {
        const duration = document.getElementById('duration').value;
        const speed = document.getElementById('speed').value;

        if (!currentDirection || !duration || !speed) {
            message = "Please complete all drive settings before pressing GO.";
        } else {
            message = `Executing drive command:\nDirection: ${currentDirection.toUpperCase()}, Duration: ${duration}s, Speed: ${speed}`;
        }
    } else if (currentCommand === "sleep") {
        message = "Robot is going to sleep...";
    }

    document.getElementById("responseDisplay").innerText = message;
}

// function sendCommand(event){
//     event.preventDefault();
//     var action
//     if(currentCommand === "drive"){
//         action = '/drive';
//         var direction = encodeURIComponent(currentDirection);
//         var duration = encodeURIComponent(document.getElementById('duration').value);
//         var speed = encodeURIComponent(document.getElementById('speed').value);
//         action += `?direction=${direction}&duration=${duration}&speed=${speed}`;

//         document.getElementById("controlCommand").action = action;
//         document.getElementById("controlCommand").submit();
//     }
// }

function sendCommand(event) {
    event.preventDefault();

    if (currentCommand === "drive") {
        const direction = encodeURIComponent(currentDirection);
        const duration = encodeURIComponent(document.getElementById('duration').value);
        const speed = encodeURIComponent(document.getElementById('speed').value);

        const url = `/telecommand?cmd=drive&direction=${direction}&duration=${duration}&speed=${speed}`;

        fetch(url, {
            method: 'PUT',
        })
        .then(response => {
            if(response.status === 200){
                return response.json();
            }
            else{
                throw new Error(response.status + ": " + response.statusText);
            }
        })
        .then(data => {
            var direction = data.direction;
            var duration = data.duration;
            var speed = data.speed;
            document.getElementById("responseDisplay").style.color = "green";
            document.getElementById("responseDisplay").innerText = "Robot is going " + direction + " for " + duration + " seconds at " + speed + " speed.";
        })
        .catch(error => {
            console.error("Error sending command:", error);
            document.getElementById("responseDisplay").style.color = "red";
            document.getElementById("responseDisplay").innerText = error.message;
        });
    }

    else if (currentCommand === "sleep") {
        const url = `/telecommand?cmd=sleep`;

        fetch(url, {
            method: 'PUT',
        })
        .then(response => {
            if(response.status === 200){
                document.getElementById("responseDisplay").style.color = "green";
                document.getElementById("responseDisplay").innerText = "Robot is going to sleep...";
            }
            else{
                throw new Error(response.status + ": "+response.statusText);
            }
        })
        .catch(error => {
            console.error("Error sending command:", error);
            document.getElementById("responseDisplay").style.color = "red";
            document.getElementById("responseDisplay").innerText = error.message;
        });
    }
    else if (currentCommand === "telemetry") {
        fetch('/telemetry_request',{
            method: 'GET',
        })
        .then(response => {
            if(response.status === 200){
                document.getElementById("responseDisplay").style.color = "green";
                document.getElementById("responseDisplay").innerText = "Telemetry Response displayed below...";
                return response.json();
            }
            else{
                throw new Error(response.status + ": "+response.statusText);
            }
        })
        .then(data =>{
            var res = "Last Packet Count: " + data.lastPktCount + "\n" + "Current Grade: " + data.currentGrade + "\n" + "Hit Count: " + data.hitCount + "\n" + "Last Drive Command: " + data.lastCmd + "\n" + "Last Duration: " + data.lastCmdValue + "\n" + "Last Speed: " + data.lastCmdSpeed;  
            document.getElementById("telemetryDisplay").innerText = res;
        })
        .catch(error => {
            console.error("Error fetching telemetry data:", error);
            document.getElementById("telemetryDisplay").style.color = "red";
            document.getElementById("telemetryDisplay").innerText = error.message;
        });
    }

}


// update this with telemetry backend 
function updateTelemetryData(data) {
    document.getElementById("telemetryDisplay").innerText = `Telemetry Data: ${data}`;
}
