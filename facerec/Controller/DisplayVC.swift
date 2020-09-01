//
//  DisplayVC.swift
//  facerec
//
//  Created by Eugene G on 9/1/20.
//  Copyright © 2020 Eugene G. All rights reserved.
//

import UIKit

class DisplayVC: UIViewController {
    
    @IBOutlet weak var imageView: UIImageView!
    
    public var frameExtractor: FrameExtractor?
    public var wrapper:Wrapper?
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        if let fe = self.frameExtractor {
            fe.stopSession()
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
        print("Failed Function: \(#function)")
    }
    
}

extension DisplayVC: FrameExtractorDelegate {
    func captured(image: UIImage) {
        
        if let wr = self.wrapper {
            self.imageView.image = wr.processImage(image)
        } else {
            print("\n⛔️⛔️⛔️ NO WRAPPER\n")
        }
        
    }
}
