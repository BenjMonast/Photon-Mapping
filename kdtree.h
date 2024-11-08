#include "common.h"

using NNQ = std::priority_queue<std::pair<float, int>>;

class KDTree {
    public:
        int split_dimension = -1;
        KDTree* left = nullptr;
        KDTree* right = nullptr;
        int photon_index = -1;
        std::vector<Photon>* photons;
        KDTree();
        KDTree(std::vector<Photon>* given_photons);
        void balance();
        void balance(std::vector<int> &photon_indeces);
        void locate_photons(Vec3f x, int k, int surface_index, NNQ &pq);
        void print();
};

KDTree::KDTree() {}

KDTree::KDTree(std::vector<Photon>* given_photons) {
    photons = given_photons;
}

void KDTree::balance() {
    std::vector<int> photon_indeces = std::vector<int>((*photons).size());
    std::iota(photon_indeces.begin(), photon_indeces.end(), 0);
    balance(photon_indeces);
}

void KDTree::balance(std::vector<int> &photon_indeces) {
    std::vector<Photon>* photons = this->photons;

    if (photon_indeces.size() == 1) {
        photon_index = photon_indeces[0];
        return;
    }

    Vec3f max_dim = {__FLT_MIN__, __FLT_MIN__, __FLT_MIN__};
    Vec3f min_dim = {__FLT_MAX__, __FLT_MAX__, __FLT_MAX__};
    const int num_photons = photon_indeces.size();

    for (int i : photon_indeces) {
        max_dim = max(max_dim, (*photons)[i].position);
        min_dim = min(min_dim, (*photons)[i].position);
    }

    Vec3f dim_sizes = max_dim - min_dim;
    int largest_dim;

    if (dim_sizes.x >= dim_sizes.y && dim_sizes.x >= dim_sizes.z) {
        largest_dim = 0;
    } else if (dim_sizes.y >= dim_sizes.z) {
        largest_dim = 1;
    } else {
        largest_dim = 2;
    }

    split_dimension = largest_dim;

    std::vector<int> sorted_photon_indeces = photon_indeces;
    std::sort(sorted_photon_indeces.begin(), sorted_photon_indeces.end(), [photons, largest_dim](const int &i1, const int &i2){
        return (*photons)[i1].position[largest_dim] < (*photons)[i2].position[largest_dim];
    });

    photon_index = sorted_photon_indeces[num_photons / 2];

    std::vector<int> s1(sorted_photon_indeces.begin(), sorted_photon_indeces.begin() + num_photons / 2);
    std::vector<int> s2(sorted_photon_indeces.begin() + num_photons / 2 + 1, sorted_photon_indeces.end());
    
    left = new KDTree(photons);
    left->balance(s1);
    
    if (s2.size() > 0) {
        right = new KDTree(photons);
        right->balance(s2);
    }
}

void KDTree::locate_photons(Vec3f x, int k, int surface_index, NNQ &pq) {
    Photon &photon = (*photons)[photon_index];
    float delta = linalg::length(photon.position - x);
    if (photon.surface_id == surface_index)
        pq.push(std::make_pair(delta, photon_index));
    if (pq.size() > k)
        pq.pop();

    // if (left != nullptr) left->locate_photons(x, k, target_normal, pq);
    // if (right != nullptr) right->locate_photons(x, k, target_normal, pq);

    if (split_dimension == -1) return;

    delta = x[split_dimension] - photon.position[split_dimension];

    if (x[split_dimension] < photon.position[split_dimension]) {
        if (left != nullptr) left->locate_photons(x, k, surface_index, pq);
    } else {
        if (right != nullptr) right->locate_photons(x, k, surface_index, pq);
    }

    if (pq.size() < k || pq.top().first > fabsf(delta)) {
        if (x[split_dimension] < photon.position[split_dimension]) {
            if (right != nullptr) right->locate_photons(x, k, surface_index, pq);
        } else {
            if (left != nullptr) left->locate_photons(x, k, surface_index, pq);
        }
    }
}