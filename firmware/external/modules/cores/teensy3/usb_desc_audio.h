#ifdef VINDOR_USB_AUDIO_MIDI
  #define SERIAL_FAKE

  #define VENDOR_ID             0x1FC9
  #define PRODUCT_ID            0x818C
#ifdef USB_ALTERNATE_PRODUCT_ID
  #define PRODUCT_ID2           0x818D
#endif

  // #define DEVICE_CLASS          0xEF
  // #define DEVICE_SUBCLASS       0x02
  // #define DEVICE_PROTOCOL       0x01

  #define MANUFACTURER_NAME     {'V','i','n','d','o','r',' ','M','u','s','i','c',',',' ','I','n','c','.'}
  #define MANUFACTURER_NAME_LEN 18
  #define PRODUCT_NAME          {'V','i','n','d','o','r'}
  #define PRODUCT_NAME_LEN      6
  #define EP0_SIZE              64
  #define NUM_ENDPOINTS         5
  #define NUM_USB_BUFFERS       16
  #define NUM_INTERFACE         4

  #define MIDI_INTERFACE        0       // MIDI
  #define MIDI_NUM_CABLES       1
  #define MIDI_TX_ENDPOINT      1
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      2
  #define MIDI_RX_SIZE          64

  #define AUDIO_INTERFACE       1       // Audio (uses 3 consecutive interfaces)
  #define AUDIO_TX_ENDPOINT     3
  #define AUDIO_TX_SIZE         180
  #define AUDIO_RX_ENDPOINT     4
  #define AUDIO_RX_SIZE         180
  #define AUDIO_SYNC_ENDPOINT   5

  #define ENDPOINT1_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT2_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT3_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT4_CONFIG      ENDPOINT_RECEIVE_ISOCHRONOUS
  #define ENDPOINT5_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
#endif

#ifdef VINDOR_USB_AUDIO_MIDI_SERIAL
  #define VENDOR_ID             0x1FC9
  #define PRODUCT_ID            0x818A
#ifdef USB_ALTERNATE_PRODUCT_ID
  #define PRODUCT_ID2           0x818B
#endif

  #define DEVICE_CLASS          0xEF
  #define DEVICE_SUBCLASS       0x02
  #define DEVICE_PROTOCOL       0x01

  #define MANUFACTURER_NAME     {'V','i','n','d','o','r',' ','M','u','s','i','c',',',' ','I','n','c','.'}
  #define MANUFACTURER_NAME_LEN 18
  #define PRODUCT_NAME          {'V','i','n','d','o','r'}
  #define PRODUCT_NAME_LEN      6
  #define EP0_SIZE              64
  #define NUM_ENDPOINTS         8
  #define NUM_USB_BUFFERS       16
  #define NUM_INTERFACE         6

  #define MIDI_INTERFACE        0       // MIDI
  #define MIDI_NUM_CABLES       1
  #define MIDI_TX_ENDPOINT      1
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      2
  #define MIDI_RX_SIZE          64

  #define AUDIO_INTERFACE       1       // Audio (uses 3 consecutive interfaces)
  #define AUDIO_TX_ENDPOINT     3
  #define AUDIO_TX_SIZE         180
  #define AUDIO_RX_ENDPOINT     4
  #define AUDIO_RX_SIZE         180
  #define AUDIO_SYNC_ENDPOINT   5

  #define CDC_IAD_DESCRIPTOR	  1
  #define CDC_STATUS_INTERFACE	4
  #define CDC_DATA_INTERFACE	  5	// Serial
  #define CDC_ACM_ENDPOINT	    6
  #define CDC_RX_ENDPOINT       7
  #define CDC_TX_ENDPOINT       8
  #define CDC_ACM_SIZE          16
  #define CDC_RX_SIZE           64
  #define CDC_TX_SIZE           64

  #define ENDPOINT1_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT2_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT3_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT4_CONFIG      ENDPOINT_RECEIVE_ISOCHRONOUS
  #define ENDPOINT5_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT6_CONFIG	ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT7_CONFIG	ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT8_CONFIG	ENDPOINT_TRANSMIT_ONLY
#endif

#ifdef VINDOR_USB_AUDIO_MIDI_KEYBOARD
  #define VENDOR_ID             0x1FC9
  #define PRODUCT_ID            0x818C
  #define DEVICE_CLASS          0xEF
  #define DEVICE_SUBCLASS       0x02
  #define DEVICE_PROTOCOL       0x01
  #define MANUFACTURER_NAME     {'V','i','n','d','o','r',' ','M','u','s','i','c',',',' ','I','n','c','.'}
  #define MANUFACTURER_NAME_LEN 18
  #define PRODUCT_NAME          {'V','i','n','d','o','r'}
  #define PRODUCT_NAME_LEN      6
  #define EP0_SIZE              64
  #define NUM_ENDPOINTS         6
  #define NUM_USB_BUFFERS       30
  #define NUM_INTERFACE         6

  #define MIDI_INTERFACE        0       // MIDI
  #define MIDI_NUM_CABLES       1  
  #define MIDI_TX_ENDPOINT      3
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      4
  #define MIDI_RX_SIZE          64

  #define KEYBOARD_INTERFACE    1       // Keyboard
  #define KEYBOARD_ENDPOINT     1
  #define KEYBOARD_SIZE         8
  #define KEYBOARD_INTERVAL     1

  #define KEYMEDIA_INTERFACE    2       // Keyboard Media Keys
  #define KEYMEDIA_ENDPOINT     2
  #define KEYMEDIA_SIZE         8
  #define KEYMEDIA_INTERVAL     4

  #define AUDIO_INTERFACE       3       // Audio (uses 3 consecutive interfaces)
  #define AUDIO_TX_ENDPOINT     5
  #define AUDIO_TX_SIZE         180
  #define AUDIO_RX_ENDPOINT     6
  #define AUDIO_RX_SIZE         180
  #define AUDIO_SYNC_ENDPOINT   7

  #define ENDPOINT1_CONFIG      ENDPOINT_TRANSIMIT_ONLY
  #define ENDPOINT2_CONFIG      ENDPOINT_TRANSIMIT_ONLY
  #define ENDPOINT3_CONFIG      ENDPOINT_TRANSIMIT_ONLY
  #define ENDPOINT4_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT5_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT6_CONFIG      ENDPOINT_RECEIVE_ISOCHRONOUS
  #define ENDPOINT7_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
#endif

#ifdef VINDOR_USB_AUDIO_MIDI_KEYBOARD_SERIAL
  #define VENDOR_ID             0x1FC9
  #define PRODUCT_ID            0x818E
  #define DEVICE_CLASS          0xEF
  #define DEVICE_SUBCLASS       0x02
  #define DEVICE_PROTOCOL       0x01
  #define MANUFACTURER_NAME     {'V','i','n','d','o','r',' ','M','u','s','i','c',',',' ','I','n','c','.'}
  #define MANUFACTURER_NAME_LEN 18
  #define PRODUCT_NAME          {'V','i','n','d','o','r'}
  #define PRODUCT_NAME_LEN      6
  #define EP0_SIZE              64
  #define NUM_ENDPOINTS         9
  #define NUM_USB_BUFFERS       30
  #define NUM_INTERFACE         8

  #define MIDI_INTERFACE        0       // MIDI
  #define MIDI_NUM_CABLES       1  
  #define MIDI_TX_ENDPOINT      5
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      6
  #define MIDI_RX_SIZE          64

  #define KEYBOARD_INTERFACE    1       // Keyboard
  #define KEYBOARD_ENDPOINT     3
  #define KEYBOARD_SIZE         8
  #define KEYBOARD_INTERVAL     1

  #define CDC_STATUS_INTERFACE  2
  #define CDC_DATA_INTERFACE    3
  #define CDC_ACM_ENDPOINT      10
  #define CDC_RX_ENDPOINT       2
  #define CDC_TX_ENDPOINT       1
  #define CDC_ACM_SIZE          16
  #define CDC_RX_SIZE           64
  #define CDC_TX_SIZE           64

  #define KEYMEDIA_INTERFACE    4       // Keyboard Media Keys
  #define KEYMEDIA_ENDPOINT     4
  #define KEYMEDIA_SIZE         8
  #define KEYMEDIA_INTERVAL     4

  #define AUDIO_INTERFACE       5       // Audio (uses 3 consecutive interfaces)
  #define AUDIO_TX_ENDPOINT     7
  #define AUDIO_TX_SIZE         180
  #define AUDIO_RX_ENDPOINT     8
  #define AUDIO_RX_SIZE         180
  #define AUDIO_SYNC_ENDPOINT   9

  #define ENDPOINT1_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT2_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT3_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT4_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT5_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT6_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT7_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT8_CONFIG      ENDPOINT_RECEIVE_ISOCHRONOUS
  #define ENDPOINT9_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT10_CONFIG     ENDPOINT_TRANSMIT_ONLY
#endif

#ifdef VINDOR_USB_AUDIO_MIDI_SEREMU
  #define VENDOR_ID             0x1FC9
  #define PRODUCT_ID            0x818F
  // #define DEVICE_CLASS          0xEF
  // #define DEVICE_SUBCLASS       0x02
  // #define DEVICE_PROTOCOL       0x01
  #define MANUFACTURER_NAME     {'V','i','n','d','o','r',' ','M','u','s','i','c',',',' ','I','n','c','.'}
  #define MANUFACTURER_NAME_LEN 18
  #define PRODUCT_NAME          {'V','i','n','d','o','r'}
  #define PRODUCT_NAME_LEN      6
  #define EP0_SIZE              64
  #define NUM_ENDPOINTS         9
  #define NUM_USB_BUFFERS       30
  #define NUM_INTERFACE         5

  #define MIDI_INTERFACE        0       // MIDI
  #define MIDI_NUM_CABLES       1  
  #define MIDI_TX_ENDPOINT      1
  #define MIDI_TX_SIZE          64
  #define MIDI_RX_ENDPOINT      2
  #define MIDI_RX_SIZE          64

  #define SEREMU_INTERFACE      1       // Serial emulation
  #define SEREMU_TX_ENDPOINT    3
  #define SEREMU_TX_SIZE        64
  #define SEREMU_TX_INTERVAL    1
  #define SEREMU_RX_ENDPOINT    4
  #define SEREMU_RX_SIZE        32
  #define SEREMU_RX_INTERVAL    2

  #define AUDIO_INTERFACE       2       // Audio (uses 3 consecutive interfaces)
  #define AUDIO_TX_ENDPOINT     5
  #define AUDIO_TX_SIZE         180
  #define AUDIO_RX_ENDPOINT     6
  #define AUDIO_RX_SIZE         180
  #define AUDIO_SYNC_ENDPOINT   7

  #define ENDPOINT1_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT2_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT3_CONFIG      ENDPOINT_TRANSMIT_ONLY
  #define ENDPOINT4_CONFIG      ENDPOINT_RECEIVE_ONLY
  #define ENDPOINT5_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
  #define ENDPOINT6_CONFIG      ENDPOINT_RECEIVE_ISOCHRONOUS
  #define ENDPOINT7_CONFIG      ENDPOINT_TRANSMIT_ISOCHRONOUS
#endif
