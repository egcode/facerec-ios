## Intro
This is iOS inference implementation of [facerec-cpp](https://github.com/egcode/facerec) repo.
 
## Installation
#### CocoaPods
You should use [CocoaPods](http://cocoapods.org/) to install libraries in `Podfile`:
```ruby
pod install
```

## **Example usage**
`jon` and `khaleesi` are in `dataset_targarien_mobile_V2.protobuf` dataset. <br>
`jamie` and `cersei` are in `dataset_lanister_mobile_V2.protobuf`.

<br><br>
<img src="readme/demo.gif" width="250">

<br>
To use your own images try [Quickstart](https://github.com/egcode/facerec#QuickStart) and then: <br>
1. Trace your model. Trace model script [here](https://github.com/egcode/facerec/blob/master/trace_model.py) <br>
2. Convert your hdf5 to protobuf. Create protobuf script [here](https://github.com/egcode/facerec/blob/master/protobuf/convert_from_h5_to_protobuf.py)


## **Test Image**

![Result](readme/test.jpg)


## **Requirements**
- Xcode 11.3
<br><br>
