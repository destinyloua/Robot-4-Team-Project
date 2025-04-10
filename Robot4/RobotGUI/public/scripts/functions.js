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

// update this with telemetry backend 
function updateTelemetryData(data) {
    document.getElementById("telemetryDisplay").innerText = `Telemetry Data: ${data}`;
}
