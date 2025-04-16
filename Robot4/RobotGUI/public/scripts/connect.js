// Check if quantity is valid
function ConnectToRobot(event){
    event.preventDefault();

    const ipAddress = document.getElementById('ipAddress').value;
    const port = document.getElementById('port').value;
    const protocol = document.getElementById('protocol').value.toUpperCase(); 
    const url = protocol === "TCP"
        ? `/connect/${ipAddress}/${port}/TCP`
        : `/connect/${ipAddress}/${port}`; 

    fetch(url, {method: 'POST'})
    .then(response => response.text()) // Handle plain text response
    .then(data => {
        console.log('Connection response:', data);
        alert(data);
        window.location.href = '/'; // Redirect to robot page
    })
    .catch(error => {
        console.error('Error connecting to robot:', error);
        alert('Failed to connect to robot');
    });
};

function DisconnectToRobot(event){
    event.preventDefault();


    const url = "/disconnect";
    fetch(url, {method: 'POST'})
    .then(response => response.text()) // Handle plain text response
    .then(data => {
        console.log('Server response:', data);
        alert(data);
        window.location.href = '/'; // Redirect to robot page
    })
    .catch(error => {
        console.error('Error disconnecting the robot:', error);
        alert('Failed to disconnect the robot');
    });
};