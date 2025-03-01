import 'package:dart_frog/dart_frog.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

final mqttClient = MqttServerClient('test.mosquitto.org', '1883');
String temp = '';
String hum = '';

Response onRequest(RequestContext context) {
  final request = context.request;
  final method = request.method;

  if (method == HttpMethod.get) {
    return doGet();
  }
  return Response(body: 'HTTP method not valid');
}

Response doGet() {
  mqttClient
    ..logging(on: true)
    ..keepAlivePeriod = 60;

  connectToMQTTBroker();

  // ignore: cascade_invocations
  mqttClient.onConnected = onConnected;
  mqttClient.onDisconnected = onDisconnected;

  return Response.json(body: {'temp': temp, 'hum': hum});
}

Future<void> connectToMQTTBroker() async {
  try {
    await mqttClient.connect();
  } catch (e) {
    mqttClient.disconnect();
  }

  if (mqttClient.connectionStatus!.state == MqttConnectionState.connected) {
    // è connesso al broker e può fare la subscribe
    await subscribeToTopic('temp_mia_api');
    await subscribeToTopic('hum_mia_api');
  } else {
    print(
      'Connessione fallita con codice ${mqttClient.connectionStatus!.state}',
    );
    mqttClient.disconnect();
  }
}

Future<void> subscribeToTopic(String topic) async {
  mqttClient.subscribe(topic, MqttQos.atLeastOnce);

  mqttClient.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
    final receivedTopic = c[0].topic;
    final message = c[0].payload as MqttPublishMessage;
    final payload =
        MqttPublishPayload.bytesToStringAsString(message.payload.message);
    print('Messaggio ricevuto su $receivedTopic: $payload');

    if (receivedTopic == 'temp_mia_api') {
      temp = payload;
    } else if (receivedTopic == 'hum_mia_api') {
      hum = payload;
    }
  });
}

void onConnected() {
  print('Client connesso al broker MQTT');
}

void onDisconnected() {
  print('Client disconnesso dal broker MQTT');
}
