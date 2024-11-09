    const char *url = "http://localhost:8000/createAccount";
    const char *json_data = "{\"username\":\"value\",\"password\":\"test\",\"email\":\"test\"}";

    char *response = callServer(url, json_data);
    if (response) {
        printf("Response JSON: %s\n", response);
        free(response); // Important: free the allocated memory
    }

    return 0;