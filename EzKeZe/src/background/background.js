var current_chat;
var current_chat_key;

//This is the key used to encode all the chats at the moment, it is temporary until the backend is up and running
current_chat_key = [115, 98, 103, 111, 108, 121, 112, 105, 101, 115, 114, 100, 97, 105, 105, 97];

console.log("fuck");

window.requestFileSystem  = window.requestFileSystem || window.webkitRequestFileSystem;

function onInitFs(fs) {
  fs.root.getFile('blaiseFUCKINGmarchetti.txt', {create: true}, function(fileEntry) {

    console.log(fileEntry.toURL());

    fileEntry.file(function(file) {
       var reader = new FileReader();

       reader.onloadend = function(e) {
         console.log(this.result);
       };

       reader.readAsText(file);
    }, errorHandler);

  }, errorHandler);
}

function errorHandler(e) {
  var msg = '';

  switch (e.code) {
    case FileError.QUOTA_EXCEEDED_ERR:
      msg = 'QUOTA_EXCEEDED_ERR';
      break;
    case FileError.NOT_FOUND_ERR:
      msg = 'NOT_FOUND_ERR';
      break;
    case FileError.SECURITY_ERR:
      msg = 'SECURITY_ERR';
      break;
    case FileError.INVALID_MODIFICATION_ERR:
      msg = 'INVALID_MODIFICATION_ERR';
      break;
    case FileError.INVALID_STATE_ERR:
      msg = 'INVALID_STATE_ERR';
      break;
    default:
      msg = 'Unknown Error';
      break;
  }

  console.log('Error: ' + msg);
}

window.webkitStorageInfo.requestQuota(PERSISTENT, 5*1024*1024, function(grantedBytes) {
  window.requestFileSystem(PERSISTENT, grantedBytes, onInitFs, errorHandler);
}, function(e) {
  console.log('Error', e);
});


//The listener for keyboard commands from the user
chrome.commands.onCommand.addListener(function(command){

  //The command to encrypt the currently selected text
  if(command == 'encrypt_userText'){

    //Send a message to the current active page that you want to encrypt text
    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
      chrome.tabs.sendMessage(tabs[0].id, {action: "encrypt_request"}, function(response){
        //When a response is received from the active page, this code executes
	      if(!response) console.log("response was null");
        //Encrypt the string that the active page sends you
        //var ciphertext = encrypt_sym_message(response.text, current_chat_key);
        var generate_keys_context = {
            request_type: "generate_rsa_keys",
            password: "P@ssw0rd"
        };
        background.naclModule.postMessage(generate_keys_context);

        /*
        //Add the encrypted text to an object with an identifier
        var encryptedmessage =  {'identifier': 'MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM', 'content': ciphertext};
        //Turn the object into a JSON formatted string
        var messageString = JSON.stringify(encryptedmessage);
        //Create an object to pass on to the active page
        var data = {action: 'encrypted_text', text: messageString};

        //Send the complete object to the active page
        chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
            chrome.tabs.sendMessage(tabs[0].id, data, function(response) {
            });
        });
        */
    });

  });
  }

  //The command to decrypt the currently selected text
  if(command == 'decrypt_userText'){

    //Send a message to the current active page that you want to decrypt text
    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
      chrome.tabs.sendMessage(tabs[0].id, {action: "decrypt_request"}, function(request){
        //Decrypt the text
        var array = request.content;
        plaintext = decrypt_sym_message(Uint8Array.from(array), current_chat_key);
        var data = {action: 'decrypted_text', text: plaintext};
      });
    });
  }
});

//Called when a message is sent straight to the extension
chrome.extension.onMessage.addListener(
  function(request, sender, sendResponse){
    console.log("Extenion message?");

    //If the request is for a deccryption
    if(request.msg == "decrypt_this"){
      //Decrypt the text and send it back
      var array = request.content;
      plaintext = decrypt_sym_message(Uint8Array.from(array), current_chat_key);
      sendResponse(plaintext);
    }
  }
);

//Open the local html when the extension is clicked
chrome.browserAction.onClicked.addListener(function(tab) {
    console.log("The Extension Button was Clicked");
    chrome.tabs.create({'url': chrome.extension.getURL('src/dashboard/index.html'), 'selected': true});
});

function attachDefaultListeners() {
    //  The NaCl module embed is created within the listenerDiv
    var listenerDiv = document.getElementById('listener');

    //  register the handle Message function as the message event handler.
    listenerDiv.addEventListener('message', handleMessage, true);
}

function handleMessage(message) {
    //  handle a message from NaCl
    console.log(message.data);

        //Add the encrypted text to an object with an identifier
        var encryptedmessage =  {'identifier': 'MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM', 'content': ciphertext};
        //Turn the object into a JSON formatted string
        var messageString = JSON.stringify(encryptedmessage);
        //Create an object to pass on to the active page
        var data = {action: 'encrypted_text', text: messageString};

        //Send the complete object to the active page
        chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
            chrome.tabs.sendMessage(tabs[0].id, data, function(response) {
            });
        });
}
