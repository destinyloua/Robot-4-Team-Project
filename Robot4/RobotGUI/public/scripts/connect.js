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
        alert('Connected to robot!');
        window.location.href = '/'; // Redirect to robot page
    })
    .catch(error => {
        console.error('Error connecting to robot:', error);
        alert('Failed to connect to robot');
    });
};