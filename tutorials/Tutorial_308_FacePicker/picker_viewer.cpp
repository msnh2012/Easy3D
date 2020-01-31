/**
 * Copyright (C) 2015 by Liangliang Nan (liangliang.nan@gmail.com)
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++
 *      library for processing and rendering 3D data. 2018.
 * ------------------------------------------------------------------
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "picker_viewer.h"
#include <easy3d/gui/picker_surface_mesh.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/viewer/drawable_triangles.h>
#include <easy3d/util/logging.h>


using namespace easy3d;


PickerViewer::PickerViewer(const std::string &title)
        : Viewer(title) {
    picker_ = new SurfaceMeshPicker(camera());
}


PickerViewer::~PickerViewer() {
    delete picker_;
}


std::string PickerViewer::usage() const {
    return ("------------ Picker Viewer usage ---------- \n"
            "Press the mouse to pick a face\n"
            "------------------------------------------ \n");
}


bool PickerViewer::mouse_press_event(int x, int y, int button, int modifiers) {
    auto model = dynamic_cast<SurfaceMesh *>(current_model());
    if (model) {
        // GLFW (same as Qt) uses upper corner for its origin while GL uses the lower corner.
        int gl_x = x;
        int gl_y = camera()->screenHeight() - 1 - y;

        // NOTE: when dealing with OpenGL, we always work in the highdpi screen space
#if defined(__APPLE__)
        gl_x = static_cast<int>(gl_x * dpi_scaling());
        gl_y = static_cast<int>(gl_y * dpi_scaling());
#endif

        auto face = picker_->pick_face(model, gl_x, gl_y);
        auto drawable = model->triangles_drawable("faces");
        auto triangle_range = model->get_face_property<std::pair<int, int> >("f:triangle_range");
        if (triangle_range && face.is_valid()) {
            const auto& range = triangle_range[face];
            drawable->set_highlight_range(range);
        }
        else
            drawable->set_highlight_range(std::make_pair(-1, -1));

        LOG_IF(ERROR, !triangle_range) << "face property 'f:triangle_range' not defined";
    }

    return Viewer::mouse_press_event(x, y, button, modifiers);
}
