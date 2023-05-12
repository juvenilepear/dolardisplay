// React Native Fetch – To make HTTP API call in React Native
// https://aboutreact.com/react-native-http-networking/

// import React in our code
import React, {useState, useEffect} from 'react';


// import all the components we are going to use
import {
  SafeAreaView,
  StyleSheet,
  View,
  TouchableOpacity,
  Text,
  FlatList,
  Button,
  Modal,
  TextInput
} from 'react-native';
import Constants from 'expo-constants';
import Checkbox from 'expo-checkbox'
import { Card } from 'react-native-paper';

const data = [
  { id: 1, txt: 'BCV', isChecked: false },
  { id: 2, txt: 'MonitorDolar', isChecked: false },
  { id: 3, txt: 'CLPtoday', isChecked: false },
];


const App = () => {
  const [products, setProducts] = React.useState(data);
  const [network, onChangeNetwork] = React.useState('');
  const [password, onChangePassword] = React.useState('');

  const handleChange = (id) => {
    let temp = products.map((product) => {
      if (id === product.id) {
        return { ...product, isChecked: !product.isChecked };
      }
      return product;
    });
    setProducts(temp);
  };

  const renderFlatList = (renderData) => {
    return (
      <FlatList
        data={renderData}
        renderItem={({ item }) => (
          <Card style={{ margin: 5 }}>
            <View style={styles.card}>
              <View
                style={{
                  flexDirection: 'row',
                  flex: 1,
                  justifyContent: 'space-between',
                }}>
                <Text>{item.txt}</Text>
                <Checkbox
                  value={item.isChecked}
                  onValueChange={() => {
                    handleChange(item.id);
                  }}
                />
              </View>
            </View>
          </Card>
        )}
      />
    );
  };

  const getDataUsingPost = () => {
    //POST json
    var sources = '';

    for (let i = 0; i < products.length; i++) {
      console.log(products[i]);
      console.log(products[i].isChecked);
      if(products[i].isChecked) {
        sources = sources + "1";
      }
      else {
        sources = sources + "0";
      }
    }
    
    var dataToSend = { sources: sources, networkSSID: network, networkPassword: password };
    //making data to send on server
    var formBody = [];

    formBody = JSON.stringify(dataToSend);
    //POST request
    fetch('http://192.168.0.24:5000/config', {
      method: 'POST', //Request Type
      body: formBody, //post body
      headers: {
        //Header Defination
        'Content-Type': 'application/json',
      },
    })
      .then((response) => response.json())
      //If response is in json then in success
      .then((responseJson) => {
        alert("Exito");
        console.log(responseJson);
      })
      //If response is not in json then in error
      .catch((error) => {
        alert(JSON.stringify(error));
        console.error(error);
      });
  };

  useEffect(() => {
    //getDataUsingPost(); code here executes on load
  }, [])

  return (
    <SafeAreaView style={{ flex: 1 }}>
      <View style={[styles.container, styles.sourcesSection]}>
      <Text style={styles.textTitle}>Tasas</Text>

      <View style={[styles.sourcesList]}>{renderFlatList(products)}</View>
      </View>

      <View style={[styles.container, styles.wifiSection]}>
      <Text style={styles.textTitle}>Credenciales Wi-Fi</Text>
      <TextInput
        style={styles.input}
        onChangeText={onChangeNetwork}
        value={network}
        placeholder='Nombre de la Red'
      />
      <Text></Text>
      <TextInput
        style={styles.input}
        onChangeText={onChangePassword}
        value={password}
        placeholder='Contraseña'
      />
      </View>

      <View style={[styles.container, styles.saveButton]}>
      <TouchableOpacity
          style={styles.buttonStyle}
          onPress={getDataUsingPost}>
          <Text style={styles.textButton}>Guardar configuración</Text>
        </TouchableOpacity>
      </View>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    //flex: 1,
    backgroundColor: 'white',
    justifyContent: 'center',
    padding: 20,
  },
  sourcesList: {
    marginTop:10,
  },
  sourcesSection: {
    marginTop:50,
  },
  textTitle: {
    fontSize: 18,
    color: 'black',
  },
  saveButton: {
    
  },
  textButton: {
    fontSize: 18,
    color: 'white',
  },
  buttonStyle: {
    alignItems: 'center',
    backgroundColor: '#54f13e',
    padding: 15,
    marginVertical: 0,
  },
  input: {
    borderColor: "#000000",
    borderWidth: 1,
    backgroundColor: '#dbd9d9',
    padding: 10,
    marginVertical: 0,
  },
  card: {
    padding: 10, 
    margin: 5,
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
  modalView: {
    margin: 20,
    backgroundColor: 'white',
    borderRadius: 20,
    padding: 5,
    justifyContent: 'space-between',
    alignItems: 'center',
    elevation: 5,
  },
  text: {
    textAlign: 'center',
    fontWeight: 'bold',
  },
});

export default App;
