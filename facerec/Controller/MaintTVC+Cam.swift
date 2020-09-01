//
//  MaintTVC+Cam.swift
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

import UIKit

extension MaintTVC {
    
    
    func initCamButton() {
        let b = UIBarButtonItem(
            title: "Cam",
            style: .plain,
            target: self,
            action: #selector(changeCamClicked(sender:))
        )
        self.navigationItem.leftBarButtonItem = b
    }

    @objc func changeCamClicked(sender: UIBarButtonItem) {
        self.changeCamAlert(title: "Change Camera", message: "")
    }
    
    func changeCamAlert(title: String, message: String) {
        let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertController.Style.alert)
        alert.addAction(UIAlertAction(title: "Front", style: UIAlertAction.Style.default, handler: { (action) in
            self.setTitle(cam: .front, fps: self.fps)
        }))
        alert.addAction(UIAlertAction(title: "Back", style: UIAlertAction.Style.default, handler: { (action) in
            self.setTitle(cam: .back, fps: self.fps)
        }))
        alert.addAction(UIAlertAction(title: "Cancel", style: UIAlertAction.Style.destructive, handler: nil))
        self.present(alert, animated: true)
    }

}
