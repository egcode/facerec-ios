//
//  MaintTVC.swift
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

import UIKit

class MaintTVC: UITableViewController {
    
    var fps:Int = 1 // Default FPS
    var cam: Cam = .front
    
    struct FileStruct {
        var title: String
        var path: String
    }
    var datasets = [FileStruct]()

    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.initFpsButton()
        self.initCamButton()
        self.setTitle(cam: self.cam, fps: self.fps)
        self.datasets = self.getFileStructsWithExtension(fileExtension: "protobuf")
    }
    
    // MARK: - Helpers
    
    func getFileStructsWithExtension(fileExtension:String) -> [FileStruct] {
        var result = [FileStruct]()
        if let urls = Bundle.main.urls(forResourcesWithExtension: fileExtension, subdirectory: nil) {
            for url in urls {
                result.append(FileStruct(title: url.lastPathComponent, path: url.path))
            }
        }
        return result
    }
    
    func setTitle(cam:Cam, fps:Int) {
        self.fps = fps
        self.cam = cam
        self.title = "CAMERA: \(cam.rawValue),  FPS: \(self.fps)"
    }

    // MARK: - Table view

    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    override func tableView(_ tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        return "Datasets"
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.datasets.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "reuseIdentifier", for: indexPath)
        cell.textLabel?.text = self.datasets[indexPath.row].title
        return cell
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let d = self.datasets[indexPath.row]
        self.showActionSheet(selectedDataset: d)
    }
    
    func showActionSheet(selectedDataset:FileStruct) {
        
        let alert = UIAlertController(title: "Model", message: "Please Select your Traced Model", preferredStyle: .actionSheet)
        
        let models = self.getFileStructsWithExtension(fileExtension: "pt")

        for model in models {
            alert.addAction(UIAlertAction(title: model.title, style: .default , handler:{ (UIAlertAction) in
                print("Model Selected: \(model.title)")
                print("Dataset Selected: \(selectedDataset.title)")
                self.transitionToDisplayVC(selectedModelPath: model.path, selectedDatasetPath: selectedDataset.path)
            }))
        }
        self.present(alert, animated: true, completion: {
        })

    }

    // MARK: - Transition
    
    func transitionToDisplayVC(selectedModelPath:String, selectedDatasetPath:String) {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        guard let vc = storyBoard.instantiateViewController(withIdentifier: "image_process_vc") as? DisplayVC else {
            assertionFailure("🚫 Could not present DisplayVC")
            return
        }
        vc.wrapper = Wrapper(paths: selectedModelPath, datasetPath: selectedDatasetPath)
        if vc.wrapper != nil {
            vc.frameExtractor = FrameExtractor(cam: self.cam, fps: self.fps)
            vc.frameExtractor?.delegate = vc
        }
        self.navigationController?.pushViewController(vc, animated: true)
    }
}
