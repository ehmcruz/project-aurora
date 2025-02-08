#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

Map::Map ()
{
	this->vertices = Mylib::Matrix<Vertex>(100, 100);

	for (uint32_t i = 0; i < this->vertices.get_nrows(); i++) {
		for (uint32_t j = 0; j < this->vertices.get_ncols(); j++) {
			this->vertices[i, j] = Vertex {
				.pos = Vector(i, j, 0)
			};
		}
	}

	// generate graphics vertices

	using enum Rect2D::VertexPositionIndex;

	const Opengl_TextureDescriptor *desc = Texture::grass.info->data.get_value<Opengl_TextureDescriptor*>();

	this->graphics_vertices.resize((this->vertices.get_nrows()-1) * (this->vertices.get_ncols()-1) * 6);

	uint32_t k = 0;
	for (uint32_t i = 0; i < this->vertices.get_nrows()-1; i++) {
		for (uint32_t j = 0; j < this->vertices.get_ncols()-1; j++) {
			GraphicsVertex *gv = &this->graphics_vertices[k];

			for (uint32_t l = 0; l < 6; l++) {
				gv[l].gvertex.pos = Vector::zero(); // local position
			}

			enum PositionIndex {
				WestSouth = 0,
				WestNorth = 1,
				EastSouth = 2,
				EastSouthRepeat = 3,
				WestNorthRepeat = 4,
				EastNorth = 5
			};

			// doing clock-wise

			// first triangle

			gv[WestSouth].offset = Vector(i, j, 0);
			gv[WestNorth].offset = Vector(i, j+1, 0);
			gv[EastSouth].offset = Vector(i+1, j, 0);

			gv[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
			gv[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);
			gv[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);

			// second triangle

			gv[EastSouthRepeat].offset = gv[EastSouth].offset;
			gv[WestNorthRepeat].offset = gv[WestNorth].offset;
			gv[EastNorth].offset = Vector(i+1, j+1, 0);

			gv[EastSouthRepeat].tex_coords = gv[EastSouth].tex_coords;
			gv[WestNorthRepeat].tex_coords = gv[WestNorth].tex_coords;
			gv[EastNorth].tex_coords = Vector(desc->tex_coords[RightTop].x, desc->tex_coords[RightTop].y, desc->atlas->texture_depth);

			// calculate normals

			Vector dir_east = gv[EastSouth].offset - gv[WestSouth].offset;
			Vector dir_north = gv[WestNorth].offset - gv[WestSouth].offset;
			gv[WestSouth].gvertex.normal = Mylib::Math::normalize( Mylib::Math::cross_product(dir_east, dir_north) );
			gv[WestNorth].gvertex.normal = gv[WestSouth].gvertex.normal;
			gv[EastSouth].gvertex.normal = gv[WestSouth].gvertex.normal;

			dir_east = gv[EastNorth].offset - gv[WestNorth].offset;
			dir_north = gv[EastNorth].offset - gv[EastSouth].offset;
			gv[EastSouthRepeat].gvertex.normal = Mylib::Math::normalize( Mylib::Math::cross_product(dir_east, dir_north) );
			gv[WestNorthRepeat].gvertex.normal = gv[EastSouthRepeat].gvertex.normal;
			gv[EastNorth].gvertex.normal = gv[EastSouthRepeat].gvertex.normal;

			k += 6;
		}
	}
}

// ---------------------------------------------------

void Map::render ()
{

}

// ---------------------------------------------------

} // end namespace Game