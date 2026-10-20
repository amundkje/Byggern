

/*
void test_canbuss(){

    canMsg test_msg = {
        .id = 1,
        .length = 4,
        .bytes = {'P','i','n','g'}
    };

    can_send(&test_msg);
    printf("Sendt Ping\n\r");

    while (1) {
        if (can_receive(&test_msg)) {
            printf("Received answer %c%c%c%c\n\r", test_msg.bytes[0], test_msg.bytes[1], test_msg.bytes[2], test_msg.bytes[3]);
        }
    }


}


*/