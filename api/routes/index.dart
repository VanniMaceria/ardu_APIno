import 'package:dart_frog/dart_frog.dart';

//eventualmente per un deploy posso usare questo -> https://dart-frog.dev/deploy/globe/ 

Response onRequest(RequestContext context) {
  return Response(body: 'Welcome to Dart Frog!');
}
